// src/http_server.c
#include "http_server.h"
#include "request.h"
#include "response.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "queue.h"

void handle_http_request(struct tcp_pcb *pcb, char *buf, size_t len) {
    // NUL-terminate safely
    if (len < REQUEST_BUFFER_SIZE) buf[len] = '\0';
    else buf[REQUEST_BUFFER_SIZE-1] = '\0';

    HttpRequest req;
    memset(&req,0,sizeof(req));
    parse_http_request(buf, &req);

    printf("[core1] Req: %s %s\n", req.method, req.path);

    int status;
    const char *body;
    if (strcmp(req.method,"GET")!=0) {
        status = 405;
        body = "<h1>405 Method Not Allowed</h1>";
    } else if (strcmp(req.path,"/")==0) {
        status = 200;
        body = "<h1>Hello, Pico W!</h1>";
    } else {
        status = 404;
        body = "<h1>404 Not Found</h1>";
    }

    generate_http_response(pcb, status, body);
    tcp_close(pcb);
    printf("[core1] Resp: %d\n", status);
}
