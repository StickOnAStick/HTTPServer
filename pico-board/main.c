#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "response.h"

#define PORT 80

// Called when data arrives
static err_t on_recv(void *arg, struct tcp_pcb *pcb,
                     struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(pcb);
        return ERR_OK;
    }
    tcp_recved(pcb, p->tot_len);

    // Very simple GET-only check; ignore `arg`
    char *data = (char*)p->payload;
    if (strncmp(data, "GET ", 4) == 0) {
        generate_http_response(pcb, NULL);
    }

    pbuf_free(p);
    tcp_close(pcb);
    return ERR_OK;
}

// Called when a new client connects
static err_t on_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, on_recv);
    return ERR_OK;
}

int main() {
    stdio_init_all();
    printf("Initializing Wi-Fi chip...\n");
    fflush(stdout);
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return 1;
    }

    // Connect to your network
    cyw43_arch_enable_sta_mode();
    cyw43_arch_wifi_connect_timeout_ms("tkwaterstorage", "kimmizukura10",
                                       CYW43_AUTH_WPA2_AES_PSK, 30 * 1000);

    // Set up a TCP listener
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, on_accept);

    printf("HTTP server running on port %d\n", PORT);

    // Main loop: poll the driver & LWIP
    while (1) {
        cyw43_arch_poll();
    }

    return 0;
}
