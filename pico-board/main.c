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
    sleep_ms(200);  // give the USB-CDC link time to enumerate

    printf("1) Initialized stdio over USB\n");
    fflush(stdout);

    printf("2) Initializing Wi-Fi chip…\n");
    fflush(stdout);
    if (cyw43_arch_init()) {
        printf("ERROR: Wi-Fi init failed\n");
        return 1;
    }

    const char *ssid = "YourNetworkSSID";
    const char *pass = "YourNetworkPassword";

    printf("3) Enabling STA mode & connecting to SSID '%s'…\n", ssid);
    fflush(stdout);
    cyw43_arch_enable_sta_mode();
    int ret = cyw43_arch_wifi_connect_timeout_ms(
        ssid, pass,
        CYW43_AUTH_WPA2_AES_PSK,
        5 * 1000   // 5 s timeout for faster feedback
    );
    if (ret) {
        printf("ERROR: join failed (%d)\n", ret);
        return 1;
    }

    printf("4) Wi-Fi connected!\n");
    fflush(stdout);

    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, on_accept);

    printf("5) HTTP server running on port %d\n", PORT);
    fflush(stdout);

    // Main loop: poll the driver & lwIP
    while (1) {
        cyw43_arch_poll();
    }

    return 0;
}
