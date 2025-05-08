#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "request.h"

void parse_http_request(char* request, HttpRequest* req){
    char* line = strtok(request, "\r\n"); // Get the first line (path version)
    if (!line) return;

    // Construct the request struct (object)
    sscanf(line, "%s %s %s", req->method, req->path, req->version); 

    req->header_count = 0;
    while((line = strtok(NULL, "\r\n")) && line[0] != '\0') {
        sscanf(
            line, 
            "%[^:]: %[^\r\n]", 
            req->headers[req->header_count].key, 
            req->headers[req->header_count].value
        );
        req->header_count++;
    }
}