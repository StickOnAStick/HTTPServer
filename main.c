#include <stdio.h>
#include <stdlib.h>  // Required for exit()
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>  // Required for macOS
#include <netinet/in.h> // Required for macOS
#include <pthread.h>

#include "inc/request.h"
#include "inc/response.h"

#define PORT 8080
#define BUFFER_SIZE 1024 // 1kb buff for incoming data
#define REQUEST_BUFFER_SIZE 256 
#define MAX_CONNECTIONS 2

struct ServerState {
    int num_connections;
    struct sockaddr_in con_buff[REQUEST_BUFFER_SIZE];
    int buff_size;
    // 8 byte padding
    pthread_mutex_t lock;  

    const int max_connections;
    const int buff_capacity;
};

struct ServerState server_state = {
    .num_connections = 0,
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .max_connections = REQUEST_BUFFER_SIZE, // Will tie to the number of cores we have.
    .buff_capacity = MAX_CONNECTIONS
};

void *handle_client(void* sock){
    int client_socket = *(int*)sock;
    free(sock); // clean up memory

    char buffer[BUFFER_SIZE];
    int bytes_recieved = read(client_socket, buffer, sizeof(buffer) - 1); // leave room for delimeter
    if (bytes_recieved < 8){
        perror("Read Error: Data too small for parsing");
        close(client_socket);
        return NULL;
    }

    buffer[bytes_recieved] = '\0';
    HttpRequest req;
    parse_http_request(buffer, &req);

    printf("Recieved request: %s %s %s\n", req.method, req.path, req.version);

    generate_http_response(client_socket, req.path);
    
    pthread_mutex_lock(&server_state.lock);
    server_state.num_connections--;
    pthread_mutex_unlock(&server_state.lock);
    close(client_socket);

    return NULL;
}


int main(){
    
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_attr_t attr;
    int opt = 1;

    memset(server_state.con_buff, 0, sizeof(server_state.con_buff)); // Clean the inital memory


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

    // Threads with this attribute will be detacted at creation.
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Listen
    if(listen(server_socket, MAX_CONNECTIONS) < 0)
    {
        perror("Listen Failed");
        close(server_socket);
        exit(EXIT_FAILURE);
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

        int* new_sock = (int*) malloc(sizeof(client_socket));
        *new_sock = client_socket;

        if(pthread_mutex_lock(&server_state.lock) != 0){
            // Could not obtain lock, drop connection.
            printf("ERR: Could not attain server_state lock! Dropping connection.");
            close(*new_sock);
            free(new_sock);
            continue;
        }
        server_state.num_connections++;
        pthread_mutex_unlock(&server_state.lock);
        

        // Thread to handle client response
        pthread_t thread;
        if(pthread_create(&thread, &attr, handle_client, new_sock)){
            printf("WARN: Could not create child thread. Dropping connection.");
            close(*new_sock);
            free(new_sock);
        }

    }
    close(server_socket);
    return 0;
}