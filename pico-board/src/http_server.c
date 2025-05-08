// src/http_server.c
#include <stdio.h>
#include <string.h>

#include "pico/cyw43_arch.h"   // for cyw43_arch_lwip_begin/end
#include "lwip/tcp.h"
#include "http_server.h"
#include "request.h"
#include "queue.h"

// Called by lwIP once all queued data is ACK'd.
// We do *not* close() here—leave the socket alive for next pipelined GET.
static err_t on_http_sent(void *arg, struct tcp_pcb *pcb, u16_t len) {
    (void)arg; (void)len;
    return ERR_OK;
}

// Build & send the response, echoing client's version and keeping the socket open.
void generate_http_response(struct tcp_pcb *pcb,
                            const char *http_version,
                            int status,
                            const char *body) {
    // 1) Pick reason phrase
    const char *reason = status == 200 ? "OK"
                        : status == 404 ? "Not Found"
                        : status == 405 ? "Method Not Allowed"
                                        : "Error";

    // 2) Build the header, echoing version and advertising Keep-Alive
    char hdr[256];
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n"  // <- persist the connection
        "Keep-Alive: timeout=5, max=100\r\n"
        "\r\n",
        status, reason,
        (int)strlen(body)
    );

    // 3) Under the lwIP lock: register sent-callback, queue data, flush
    cyw43_arch_lwip_begin();
      tcp_sent (pcb, on_http_sent);
      tcp_write(pcb, hdr,  hlen,         TCP_WRITE_FLAG_COPY);
      tcp_write(pcb, body, strlen(body), TCP_WRITE_FLAG_COPY);
      tcp_output(pcb);
    cyw43_arch_lwip_end();

    // DO NOT call tcp_close() here!
}

// Parse and dispatch each request on core1
void handle_http_request(struct tcp_pcb *pcb, char *buffer, size_t length) {
    // Ensure buffer is NUL-terminated
    buffer[length < REQUEST_BUFFER_SIZE-1 ? length : REQUEST_BUFFER_SIZE-1] = '\0';

    HttpRequest req;
    memset(&req, 0, sizeof(req));
    parse_http_request(buffer, &req);

    printf("[core1] Req: %s %s\n", req.method, req.path);

    int status;
    const char *body;
    if (strcmp(req.method, "GET") != 0) {
        status = 405; body = "<h1>405 Method Not Allowed</h1>";
    } else if (strcmp(req.path, "/") == 0) {
        status = 200; body = "<h1>Hello, Pico W!</h1>";
    } else {
        status = 404; body = "<h1>404 Not Found</h1>";
    }

    // Echo back the exact HTTP-version string the client sent
    generate_http_response(pcb, req.version, status, body);
    printf("[core1] Resp: %d\n", status);
}
