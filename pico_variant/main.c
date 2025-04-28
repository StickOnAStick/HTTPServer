#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "queue.h"
#include "core0.h"
#include "core1.h"

int main(){
    queue_init(&request_queue);
    multicore_launch_core1(core1_main);
    core0_main();

    return 0;
}