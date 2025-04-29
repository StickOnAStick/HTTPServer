#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "queue.h"
#include "core0.h"
#include "core1.h"

#define SSID "SomeNetworkName"
#define PASS "SomePassword"



int main(){
    stdio_init_all();

    printf("Initalizing IO and WiFi...\n");
    sleep_ms(10000);
    queue_init(&request_queue);
   


    if(cyw43_arch_init()){
        printf("WiFi init failed!\n");
        return 1;
    }

    printf("Connecting to WiFi.. SSID: %s\n", SSID);

    if(cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)){
        printf("WiFi failed to connect!");
        return 1;
    }

    printf("WiFi Connected!\n");
    printf("Device IP address: %s\n", ip4addr_ntoa(&netif_default->ip_addr));

    multicore_launch_core1(core1_main);
    core0_main();

    return 0;
}