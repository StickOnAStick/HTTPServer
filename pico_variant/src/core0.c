#include "core0.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "queue.h"
#include "types.h"

#define SSID "YourWifiSSID"
#define PASS "YourPass"
#define PORT 80
#define LED_PIN 18 // This maps to GPIO 18


static err_t recv_cb(void *arg, struct tcp_pcb* pcb, struct pbuf *p, err_t err){
    if(!p){
        tcp_close(pcb);
        return ERR_OK;
    }

    request_t req;
    memset(&req, 0, sizeof(req));
    // Set the protocol control block to the incoming protocol control block
    req.pcb = pcb;
    // Set the buffer to the incoming request's payload buffer
    memset(req.buffer, p->payload, p->len < REQUEST_BUFFER_SIZE ? p->len : REQUEST_BUFFER_SIZE);
    req.length = p->len;

    queue_push(&request_queue, &req);

    pbuf_free(p);
    return ERR_OK;
}

static err_t accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err){
    tcp_recv(newpcb, recv_cb);
    return ERR_OK;
}

void core0_main(){
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Off initally.

    if(cyw43_arch_init()){
        printf("WiFi init failed!\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    const char* dev_name = "pico";
    netif_set_hostname(netif_default, dev_name); // Set the device name so we can ping it with `pi.local`

    if(cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 30000)){
        printf("WiFi failed to connect!");
        return;
    }

    printf("Wifi Connected!\tDevice name: %s\tIP Addr: %s", dev_name, ip4addr_ntoa(&netif_default->ip_addr));

    struct tcp_pcb* pcb = tcp_new();
    if(!pcb){
        printf("Failed to create protocol control buffer!\n");
        return;
    }

    if(tcp_bind(pcb, IP_ADDR_ANY, PORT) != ERR_OK){
        printf("Failed to bind!\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, accept_cb);

    while(true){
        cyw43_arch_poll();
        sleep_ms(1);
    }

    cyw43_arch_deinit();
}