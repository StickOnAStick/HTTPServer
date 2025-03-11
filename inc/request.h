#ifndef REQUEST_H
#define REQUEST_H

#include "header.h"

typedef struct {
    char method[8]; // GET, POST, etc...
    char path[256]; // The path requested (ex: /data/index.html)
    char version[16]; // HTTP/0.9, HTTP/1.0
    HttpHeader headers[10]; // Up to 10 headers.
    int header_count;  
} HttpRequest;

// Parses an HTTP request and constructs the HttpRequest.
void parse_http_request(char *request, HttpRequest *req);

#endif