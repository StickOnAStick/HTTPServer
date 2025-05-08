#pragma once

#include <stddef.h>
#include "lwip/tcp.h"

/**
 * Build & queue an HTTP response (status, headers, body),
 * echoing back exactly the client's HTTP-version string,
 * advertising keep-alive, and *not* closing the socket.
 *
 * @param pcb           the TCP control block
 * @param http_version  the literal version string from the request
 *                       (e.g. "HTTP/1.0" or "HTTP/1.1")
 * @param status        numeric HTTP status code (200, 404, 405, etc.)
 * @param body          NUL-terminated response body
 */
void generate_http_response(struct tcp_pcb *pcb,
                            const char *http_version,
                            int status,
                            const char *body);

/**
 * Top-level dispatcher: parses the raw request in `buffer`,
 * decides on status + body, then calls generate_http_response().
 *
 * @param pcb     the TCP control block
 * @param buffer  NUL-terminated request data
 * @param length  number of bytes in buffer (not including NUL)
 */
void handle_http_request(struct tcp_pcb *pcb,
                         char *buffer,
                         size_t length);

