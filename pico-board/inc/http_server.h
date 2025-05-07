#pragma once

#include "lwip/tcp.h"
#include <stddef.h>

/// Parse a raw HTTP buffer and send a response, then close the PCB.
void handle_http_request(struct tcp_pcb *pcb, char *buffer, size_t length);
