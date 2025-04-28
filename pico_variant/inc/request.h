#pragma once

#include "lwip/tcp.h"

void handle_http_request(struct tcp_pcb *pcb, const char *data, size_t len);