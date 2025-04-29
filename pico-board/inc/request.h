#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
    char method[8];
    char path[128];
    char version[16];
} HttpRequest;

void parse_http_request(const char *raw, HttpRequest *req);

#endif
