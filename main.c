#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "request.h"
#include "response.h"

#define PORT 8080
#define BUFFER_SIZE 1024 // 1kb buff


void handle_client(int client_socket){
    char buffer[BUFFER_SIZE];
    int bytes_recieved = read(client_socket, buffer, sizeof(buffer) - 1); // leave room for delimeter

    if (bytes_recieved < 8){
        perror("Read Error: Data too small for parsing");
        close(client_socket);
        return;
    }

    buffer[bytes_recieved] = "\0";
    HttpRequest req;
    parse_http_request(buffer, &req);

    printf("Recieved request: %s %s %s\n", req.method, req.path, req.version);

    generate_http_response(client_socket, req.path);

    
    close(client_socket);
}


int main(int argc, char* argv){
    
    while(true){
        // Listen
      
    }

    return 0;
}