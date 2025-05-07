#ifndef RESPONSE_H
#define RESPONSE_H

#include "lwip/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Generate a full HTTP/1.1 response over the raw TCP API.
/// @param pcb          The lwIP TCP protocol control block to write to.
/// @param status_code  Numeric status code (e.g. 200, 404, 500).
/// @param body         NUL-terminated response body (HTML or other).
void generate_http_response(struct tcp_pcb *pcb, int status_code, const char *body);

#ifdef __cplusplus
}
#endif

#endif // RESPONSE_H
