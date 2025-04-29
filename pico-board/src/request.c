#include "request.h"
#include <string.h>
#include <stdio.h>

void parse_http_request(const char *raw, HttpRequest *req) {
    sscanf(raw, "%s %s %s", req->method, req->path, req->version);
}