#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


#include "inc/request.h"
#include "inc/response.h"

#define BUFFER_SIZE 1024 // 1kb buff
#define MAX_CONNECTIONS 5 // arbitrary, we should test how many it can take.

#ifdef __linux__
    #include <arpa/inet.h>
    #define PORT 8080
    #define PLATFORM "Linux"
#elif defined(PICO_BUILD)
    #include "pico-sdk/cyw43_arch.h"
    #include "lwip/sockets.h"
    #define PORT 80
    #define PLATFORM "PicoW"
#endif



void handle_client(int client_socket){
    char buffer[BUFFER_SIZE];
    int bytes_recieved = read(client_socket, buffer, sizeof(buffer) - 1); // leave room for delimeter

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
    printf("Startting HTTP server on port %s for %s\n", PORT, PLATFORM);

    #ifdef PICOBUILD
        if(cyw43_arch_init()){
            printf("Failed to initalize WiFi\n");
            exit(EXIT_FAILURE);
        }
        cyw43_arch_enable_sta_model();
        printf("WiFi initalized on PicoW\n");
    #endif

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

        handle_client(client_socket);
    }
    close(server_socket);
    return 0;
}