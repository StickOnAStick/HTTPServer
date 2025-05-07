#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "response.h"
#include "request.h"

#define PORT          80
#define MAX_REQ_LEN 1024

// -----------------------------------------------------------------------------
// File‐scope state for picking the strongest AP
// -----------------------------------------------------------------------------
static struct {
    uint8_t  bssid[6];
    uint8_t  channel;
    int16_t  rssi;
} best_ap = { .rssi = -128 };

static int scan_best_cb(void *env, const cyw43_ev_scan_result_t *r) {
    const char *target = (const char*)env;
    if (!r) return 0;
    if (r->ssid_len == strlen(target)
        && memcmp(r->ssid, target, r->ssid_len) == 0
        && r->rssi > best_ap.rssi) {
        best_ap.rssi    = r->rssi;
        best_ap.channel = r->channel;
        memcpy(best_ap.bssid, r->bssid, 6);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Handle incoming data on a connection
// -----------------------------------------------------------------------------
static err_t on_recv(void *arg, struct tcp_pcb *pcb,
                     struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(pcb);
        return ERR_OK;
    }
    tcp_recved(pcb, p->tot_len);

    // Copy into a local buffer and NUL-terminate
    int len = p->tot_len;
    if (len > MAX_REQ_LEN - 1) len = MAX_REQ_LEN - 1;
    char buf[MAX_REQ_LEN];
    memcpy(buf, p->payload, len);
    buf[len] = '\0';

    // Parse the request
    HttpRequest req;
    memset(&req, 0, sizeof(req));      // zero out all fields
    parse_http_request(buf, &req);

    // Route and respond
    if (strcmp(req.method, "GET") != 0) {
        const char *body = "405 Method Not Allowed";
        generate_http_response(pcb, 405, body);
    } else if (strcmp(req.path, "/") == 0) {
        const char *body = "Hello, Pico W!";
        generate_http_response(pcb, 200, body);
    } else {
        const char *body = "404 Not Found";
        generate_http_response(pcb, 404, body);
    }

    pbuf_free(p);
    tcp_close(pcb);
    return ERR_OK;
}

// -----------------------------------------------------------------------------
// New connection has been accepted
// -----------------------------------------------------------------------------
static err_t on_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, on_recv);
    return ERR_OK;
}

// -----------------------------------------------------------------------------
// Application entry point
// -----------------------------------------------------------------------------
int main() {
    stdio_init_all();
    sleep_ms(2000);  // let macOS enumerate the USB-CDC port

    printf("stdio over USB ready\n"); fflush(stdout);

    // 1) Initialize Wi-Fi
    if (cyw43_arch_init()) {
        printf("ERROR: Wi-Fi init failed\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    // 2) Scan for best BSSID of our SSID
    const char *ssid = "tkwaterstorage";
    const char *pass = "kimmizukura10";

    printf("Scanning for '%s'…\n", ssid); fflush(stdout);
    cyw43_wifi_scan_options_t opts = {0};
    if (cyw43_wifi_scan(&cyw43_state, &opts, (void*)ssid, scan_best_cb)) {
        printf("ERROR: scan start failed\n");
        return 1;
    }
    while (cyw43_wifi_scan_active(&cyw43_state)) {
        cyw43_arch_poll();
        sleep_ms(50);
    }
    if (best_ap.rssi < -120) {
        printf("ERROR: SSID '%s' not found\n", ssid);
        return 1;
    }
    printf("Chosen BSSID %02x:%02x:%02x:%02x:%02x:%02x  CH %u  RSSI %d\n",
           best_ap.bssid[0], best_ap.bssid[1], best_ap.bssid[2],
           best_ap.bssid[3], best_ap.bssid[4], best_ap.bssid[5],
           best_ap.channel, best_ap.rssi);
    fflush(stdout);

    // 3) Join that AP
    printf("Joining…\n"); fflush(stdout);
    if (cyw43_wifi_join(&cyw43_state,
                        strlen(ssid), (const uint8_t*)ssid,
                        strlen(pass), (const uint8_t*)pass,
                        CYW43_AUTH_WPA2_AES_PSK,
                        best_ap.bssid,
                        best_ap.channel)) {
        printf("ERROR: join API failed\n");
        return 1;
    }

    // 4) Configure a static IP
    struct netif *netif = netif_default;
    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr,  192,168,0,42);
    IP4_ADDR(&netmask, 255,255,255,0);
    IP4_ADDR(&gw,      192,168,0,1);
    netif_set_addr(netif, &ipaddr, &netmask, &gw);
    printf("Static IP: %s  GW: %s\n",
           ip4addr_ntoa(&netif->ip_addr),
           ip4addr_ntoa(&netif->gw));
    fflush(stdout);

    // 5) Start HTTP server
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, PORT);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, on_accept);

    printf("HTTP server running on port %d\n", PORT);
    fflush(stdout);

    // 6) Poll & heartbeat
    while (1) {
        cyw43_arch_poll();
        sleep_ms(1000);
        printf("."); fflush(stdout);
    }

    return 0;
}
