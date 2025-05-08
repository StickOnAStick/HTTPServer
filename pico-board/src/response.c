#include "response.h"
#include <stdio.h>
#include <string.h>

/// Lookup the standard reason phrase for a status code.
static const char *reason_phrase(int code) {
    switch (code) {
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default:  return "Unknown";
    }
}

#include "pico/cyw43_arch.h"   // for cyw43_arch_lwip_begin/end
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

// new signature with status code:
void generate_http_response(struct tcp_pcb *pcb, int status, const char *body) {
    char hdr[256];
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "\r\n",
        status,
        (status == 200 ? "OK" :
         status == 404 ? "Not Found" :
         status == 405 ? "Method Not Allowed" : ""),
        (int)strlen(body));

    // 1) write under lwIP lock
    cyw43_arch_lwip_begin();
    tcp_write(pcb, hdr,  hlen,         TCP_WRITE_FLAG_COPY);
    tcp_write(pcb, body, strlen(body), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);  // <- force lwIP to send now
    cyw43_arch_lwip_end();

    // 2) give it a moment on the air
    sleep_ms(50);

    // 3) close under lwIP lock
    cyw43_arch_lwip_begin();
    tcp_close(pcb);
    cyw43_arch_lwip_end();
}

