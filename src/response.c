#include <stdio.h>
#include <unistd.h>
#include "response.h"

#define DATA_DIR "./data/" // Directory for files we can fetch
#define BUFFER_SIZE 1024   // Hack, but make this a constant across files.

// Helper function to check if a file exists
int file_exists(const char *filename){
    FILE *file = fopen(filename, "r");
    if (file){
        fclose(file);
        return 1;
    }
    return 0;
}

char* read_file(const char *filename, size_t *size){
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    char *buffer = malloc(*size + 1);
    if (!buffer){
        // Heap allocation failed
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *size, file);  // Read 1 byte chunks SIZE times.
    buffer[*size] = '\0';           // Set the last position to deliminator.

    fclose(file);
    return buffer;
}


void generate_http_response(int client_socket, const char* path){
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", DATA_DIR, path+1); // Remove leading '/'
    
    if(strcmp(path, "/") == 0){
        // These act like API routes. If we just hit "http://{device_ip}:8080/" we get "Hello World" as the response
        const char *body = "Hello World\n"; 
        dprintf(
            client_socket,
            "HTTP/1.1 200 OK\r\n"            // /r returns carriage, /n is new line (Set cursor to beginning, start new line)
            "Content-Type: text/plain\r\n"
            "Content-length: %zu\r\n"        // sizeof(body)
            "Connection: close\r\n"  
            "\r\n"
            "%s",                            // body
            strlen(body),
            body
        );
    } else if (file_exists(full_path)){

        size_t file_size;
        char *file_content = read_file(full_path, &file_size);

        if(file_content){
            dprintf(
                client_socket,
                "HTTP/1.1 200 OK\r\n"            // /r returns carriage, /n is new line (Set cursor to beginning, start new line)
                "Content-Type: text/html\r\n"
                "Content-length: %zu\r\n"        // sizeof(file)
                "Connection: close\r\n"  
                "\r\n",
                file_size
            );
            write(client_socket, file_content, file_size); // File cannot be sent as text, send separately.
            free(file_content);
        }
    } else {
        // default case, location doesn't exist
        // 404
        const char *not_found = "404 Not Found\n";
        dprintf(
            client_socket,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            strlen(not_found),
            not_found
        );
    }
}