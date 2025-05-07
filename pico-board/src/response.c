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

void generate_http_response(struct tcp_pcb *pcb, int status_code, const char *body) {
    const char *reason = reason_phrase(status_code);
    size_t body_len = body ? strlen(body) : 0;

    // Build and send the status line + headers
    char hdr[256];
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        status_code, reason, body_len);

    tcp_write(pcb, hdr,   hlen,       TCP_WRITE_FLAG_COPY);

    // Send the body (if any)
    if (body_len) {
        tcp_write(pcb, body, body_len, TCP_WRITE_FLAG_COPY);
    }
}
