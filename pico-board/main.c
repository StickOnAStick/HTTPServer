#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"

#include "queue.h"
#include "core0.h"
#include "core1.h"

#define PORT 80

// -----------------------------------------------------------------------------
// Scan state (unchanged)
// -----------------------------------------------------------------------------
static struct {
    uint8_t  bssid[6];
    uint8_t  channel;
    int16_t  rssi;
} best_ap = { .rssi = -128 };

static int scan_best_cb(void *env, const cyw43_ev_scan_result_t *r) {
    const char *target = (const char*)env;
    if (!r) return 0;
    if (r->ssid_len == strlen(target) &&
        memcmp(r->ssid, target, r->ssid_len) == 0 &&
        r->rssi > best_ap.rssi) {
        best_ap.rssi    = r->rssi;
        best_ap.channel = r->channel;
        memcpy(best_ap.bssid, r->bssid, 6);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Application entry point
// -----------------------------------------------------------------------------
int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("stdio over USB ready\n"); fflush(stdout);

    // 1) Wi-Fi init
    if (cyw43_arch_init()) {
        printf("ERROR: Wi-Fi init failed\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    // 2) Scan for your SSID
    const char *ssid = "tkwaterstorage";
    const char *pass = "kimmizukura10";
    printf("Scanning for '%s' …\n", ssid); fflush(stdout);
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
    printf("Picked BSSID %02x:%02x:%02x:%02x:%02x:%02x  CH %u  RSSI %d\n",
           best_ap.bssid[0], best_ap.bssid[1], best_ap.bssid[2],
           best_ap.bssid[3], best_ap.bssid[4], best_ap.bssid[5],
           best_ap.channel, best_ap.rssi);
    fflush(stdout);

    // 3) Join with built-in polling (30 s timeout)
    printf("Connecting to '%s' …\n", ssid); fflush(stdout);
    int ret = cyw43_arch_wifi_connect_timeout_ms(
        ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30 * 1000
    );
    if (ret) {
        printf("ERROR: failed to join (code %d)\n", ret);
        return 1;
    }
    printf("Wi-Fi link up!\n"); fflush(stdout);

    // 4) Start DHCP under the lwIP lock
    struct netif *netif = netif_default;
    cyw43_arch_lwip_begin();
    dhcp_start(netif);
    cyw43_arch_lwip_end();

    // 5) Wait for a non-zero IP
    printf("Waiting for DHCP lease…\n"); fflush(stdout);
    for (int i = 0; i < 200; ++i) {
        cyw43_arch_poll();
        cyw43_arch_lwip_begin();
        ip4_addr_t addr = netif->ip_addr;
        cyw43_arch_lwip_end();
        if (!ip4_addr_isany_val(addr)) break;
        sleep_ms(50);
    }

    // 6) Print the assigned IP
    ip4_addr_t final_ip = netif->ip_addr;
    printf("Got IP address: %s\n", ip4addr_ntoa(&final_ip));
    fflush(stdout);

    // 7) Kick off your two-core HTTP server
    queue_init(&request_queue);
    multicore_launch_core1(core1_main);
    printf("Launched core1_main()\n"); fflush(stdout);
    core0_main();  // never returns

    return 0;
}
