
#include "queue.h"
#include "core0.h"
#include "core1.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#define SSID "SticksSpot"
#define PASS "xyknfqg4"

// Internal tracking
static int best_rssi = -1000;  // Very low initial RSSI
static char best_ssid[33] = {0}; // SSID max length is 32 + 1 for null terminator
static uint8_t best_channel = 0;

// WiFi scan callback
static int wifi_scan_callback(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("Found SSID: %.*s | RSSI: %d | Channel: %d\n", 
               result->ssid_len, result->ssid, result->rssi, result->channel);

        // Match and find best RSSI (same logic you had)
        if (result->ssid_len == strlen(SSID) &&
            strncmp((const char *)result->ssid, SSID, result->ssid_len) == 0) {
            if (result->channel >= 1 && result->channel <= 14) { // 2.4 GHz
                if (result->rssi > best_rssi) {
                    best_rssi = result->rssi;
                    memcpy(best_ssid, result->ssid, result->ssid_len);
                    best_ssid[result->ssid_len] = '\0'; // Null terminate
                    best_channel = result->channel;
                }
            }
        }
    } else {
        printf("WiFi scan complete!\n");
    }

    return 0; // Always return 0 for success
}

void wifi_scan() {
    printf("Starting WiFi scan...\n");

    cyw43_wifi_scan_options_t scan_options;
    memset(&scan_options, 0, sizeof(scan_options));


    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, wifi_scan_callback);
    if (err != 0) {
        printf("WiFi scan failed with error: %d\n", err);
    }
}

int main(){
    stdio_init_all();
    sleep_ms(10000);

    printf("Initalizing IO and WiFi...\n");
    queue_init(&request_queue);
   
    if(cyw43_arch_init()){
        printf("WiFi init failed!\n");
        return 1;
    }

    // Start scan
    //wifi_scan();

    //Wait for scan results to complete
    // absolute_time_t start = get_absolute_time();
    // while (absolute_time_diff_us(start, get_absolute_time()) < 10 * 1000 * 1000) { // 10 seconds timeout
    //     cyw43_arch_poll();
    //     sleep_ms(50);
    // }

    // if (best_rssi == -1000) {
    //     printf("Target SSID '%s' not found on 2.4GHz network.\n", SSID);
    //     return 1;
    // }
    
    printf("Connecting to WiFi.. SSID: %s\n", SSID);
    cyw43_arch_enable_sta_mode();

    int err = cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 10000);
    if(err){
        printf("WiFi failed to connect! %d\n", err);
        return 1;
    }

    // Poll for DHCP, WPA handshake, etc.
    printf("Polling for IP...\n");
    for (int i = 0; i < 200; ++i) {
        cyw43_arch_poll();
        sleep_ms(50);
    }

    int link_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    printf("WiFi link status: %d\n", link_status);

    if (link_status != CYW43_LINK_UP) {
        switch (link_status) {
            case CYW43_LINK_DOWN: printf("→ Link down (not connected)\n"); break;
            case CYW43_LINK_JOIN: printf("→ Joined, not authenticated\n"); break;
            case CYW43_LINK_NOIP: printf("→ Authenticated, waiting for DHCP\n"); break;
            case CYW43_LINK_BADAUTH: printf("→ Auth failed (bad pass or WPA3)\n"); break;
            case CYW43_LINK_FAIL: printf("→ Internal link failure\n"); break;
            default: printf("→ Unknown link state\n"); break;
        }
        return 1;
    }
    
    // Check IP
    const char* ip = ip4addr_ntoa(&netif_default->ip_addr);
    if (strcmp(ip, "0.0.0.0") == 0) {
        printf("Connected, but no IP assigned (DHCP failure)\n");
        return 1;
    }

    printf("WiFi Connected!\n");
    printf("Device IP address: %s\n", ip);

    multicore_launch_core1(core1_main);
    core0_main();

    return 0;
}