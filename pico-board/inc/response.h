#ifndef RESPONSE_H
#define RESPONSE_H

#include "lwip/tcp.h"    // raw‐API TCP

// now takes a tcp_pcb*, not a netconn
void generate_http_response(struct tcp_pcb *pcb, const char *path);

#endif /* RESPONSE_H */