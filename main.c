#include <stdio.h>
#include <stdlib.h>  // Required for exit()
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>  // Required for macOS
#include <netinet/in.h> // Required for macOS
#include <sys/time.h>
#include <errno.h>


#include "inc/request.h"
#include "inc/response.h"
#include "inc/queue.h"
#include "inc/worker.h"

#define PORT 8080
#define BUFFER_SIZE 1024 // 1kb buff
#define MAX_CONNECTIONS SOMAXCONN
#define NUM_THREADS 12
#define SOCKET_TIMEOUT_SECS 1

pthread_t threads[NUM_THREADS];
queue_t TaskQueue;

void handle_client(void* arg){
    int client_socket = *(int*)arg;
    free(arg); // Free right away.
    char buffer[BUFFER_SIZE];
    int bytes_recieved = read(client_socket, buffer, sizeof(buffer) - 1); // leave room for delimeter

    if(bytes_recieved <= 0){
        if(bytes_recieved == 0){
            fprintf(stderr, "Client disconnected (fd: %d)\n", client_socket);
        } else if (errno == EAGAIN || errno == EWOULDBLOCK){
            fprintf(stderr, "Read timeout on socket %d\n", client_socket);
        }else {
            perror("Read failed");
        }
        close(client_socket);
        return;
    }

    if (bytes_recieved < 8){
        perror("Read Error: Data too small for parsing");
        close(client_socket);
        return;
    }

    buffer[bytes_recieved] = '\0';
    HttpRequest req;
    parse_http_request(buffer, &req);

    printf("Recieved request: %s %s %s\n", req.method, req.path, req.version);

    generate_http_response(client_socket, req.path);
    
    close(client_socket);
}


int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int opt = 1;

    // Create Socket for connections
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow reuse of local addresses
    // Needed for Devs, otherwise, we must wait until the OS releases the Port. When? Who knows!
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Failed to set SocketOptions.");
        exit(EXIT_FAILURE);
    }

    // Bind address
    server_addr.sin_family = AF_INET;  // IPv4, I don't want to deal with IPv6 extras & security
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept on any avail. interface (Wi-Fi, Ethernet, etc..)
    server_addr.sin_port = htons(PORT); // Set netowrk byte-order to big-endian

    if(bind(
            server_socket, 
            (struct sockaddr *)&server_addr, 
            sizeof(server_addr)
        ) < 0 ) 
    {
        perror("Bind Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen
    if(listen(server_socket, MAX_CONNECTIONS) < 0)
    {
        perror("Listen Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Initalize Task queue
    queue_init(&TaskQueue);
    // Launch worker threads
    for(int i = 0; i < NUM_THREADS; ++i){
        pthread_create(&threads[i], NULL, worker_thread, (void*)&TaskQueue);
        pthread_detach(threads[i]);
    }

    printf("HTTP Server running on port %d...\n", PORT);

    while(1){
        client_socket = accept(
            server_socket, 
            (struct sockaddr*)&client_addr, 
            &client_addr_len
        );
        if(client_socket < 0){
            perror("Accept failed.");
            continue; // Brilliant error handling
        }

        // Set socket timeouts for read/write
        struct timeval timeout = { .tv_sec = SOCKET_TIMEOUT_SECS, .tv_usec=0};
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        int* socket_pointer = malloc(sizeof(int));
        if(!socket_pointer){
            perror("malloc: could not create socket on heap");
            close(client_socket);
            continue;
        }
        *socket_pointer = client_socket;

        
        task_t task = { .function=handle_client, .arg=socket_pointer };
        if(queue_push(&TaskQueue, task)){
            const char* body = "503 Service Unavailable\n";
            //printf("Reuqest dropped: %d", *socket_pointer);
            dprintf(client_socket,
                "HTTP/1.1 503 Service Unavailable\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %zu\r\n"
                "Connection: close\r\n"
                "\r\n"
                "%s",
                strlen(body), body
            );
            close(client_socket);
            free(socket_pointer); // Prevent memory leak
        }
        //printf("Queue size: %d\n", TaskQueue.count);
    }
    close(server_socket);
    return 0;
}