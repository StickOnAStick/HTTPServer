#include "response.h"
#include <string.h>
#include <stdio.h>

void generate_http_response(struct tcp_pcb *pcb, const char *path) {
    const char *body = "<html><body><h1>Hello, Pico W!</h1></body></html>";
    char hdr[256];
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "\r\n",
        (int)strlen(body));

    // send header + body via raw TCP API
    tcp_write(pcb, hdr,   hlen,         TCP_WRITE_FLAG_COPY);
    tcp_write(pcb, body,  strlen(body), TCP_WRITE_FLAG_COPY);
}
