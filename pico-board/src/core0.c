// src/core0.c

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"   // ← for cyw43_arch_poll()
#include "lwip/tcp.h"
#include "lwip/pbuf.h"         // for struct pbuf
#include "queue.h"
#include "request.h"

#define PORT 80

// receive callback: copy, null-terminate, enqueue
static err_t recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(pcb);
        return ERR_OK;
    }
    tcp_recved(pcb, p->tot_len);
    request_t req;
    size_t copy_len = p->len < REQUEST_BUFFER_SIZE - 1
                          ? p->len
                          : REQUEST_BUFFER_SIZE - 1;
    memcpy(req.buffer, p->payload, copy_len);
    req.buffer[copy_len] = '\0';      // NUL-terminate!
    req.length = copy_len;
    req.pcb    = pcb;

    if (!queue_push(&request_queue, &req)) {
        printf("[core0] queue full—dropping\n");
        tcp_close(pcb);
    } else {
        printf("[core0] enqueued: %.*s\n", (int)copy_len, req.buffer);
    }

    pbuf_free(p);
    return ERR_OK;
}

// accept callback: install our recv_cb
static err_t accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, recv_cb);
    return ERR_OK;
}

// core0_main: listen, accept, poll Wi-Fi
void core0_main(void) {
  struct tcp_pcb *listener = tcp_new();
  tcp_bind(listener, IP_ADDR_ANY, PORT);
  listener = tcp_listen_with_backlog(listener, 50);
  tcp_accept(listener, accept_cb);
  // no polling loop needed—background thread drives LWIP
  while (true) cyw43_arch_poll();
}


