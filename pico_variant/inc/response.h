#pragma once

#include "lwip/tcp.h"

void send_http_response(struct tcp_pcb *pcb, const char* path);
void send_404_response(struct tcp_pcb* pcb);
