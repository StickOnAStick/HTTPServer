// src/core0.c
#include "core0.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"     // for struct pbuf
#include "queue.h"
#include "request.h"       // for HttpRequest and parse_http_request()

#define PORT    80
#define LED_PIN 18

// — recv callback: enqueue incoming HTTP requests —
static err_t recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(pcb);
        return ERR_OK;
    }

    request_t req;
    memset(&req, 0, sizeof(req));
    req.pcb = pcb;
    int copy_len = p->len < REQUEST_BUFFER_SIZE ? p->len : REQUEST_BUFFER_SIZE;
    memcpy(req.buffer, p->payload, copy_len);
    req.length = copy_len;

    queue_push(&request_queue, &req);

    pbuf_free(p);
    return ERR_OK;
}

// — accept callback: install recv_cb on a newly accepted PCB —
static err_t accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, recv_cb);
    return ERR_OK;
}

// — this is your core0 “main” loop —
void core0_main() {
    // LED setup for visual feedback (optional)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    // Create the listening PCB
    struct tcp_pcb *listener = tcp_new();
    if (!listener) {
        printf("Failed to create PCB\n");
        return;
    }
    if (tcp_bind(listener, IP_ADDR_ANY, PORT) != ERR_OK) {
        printf("Failed to bind to port %d\n", PORT);
        return;
    }
    listener = tcp_listen(listener);
    tcp_accept(listener, accept_cb);

    // Poll Wi-Fi & keep core alive
    while (true) {
        cyw43_arch_poll();
        sleep_ms(10);
    }
}
