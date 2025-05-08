// src/core1.c
#include <stdio.h>
#include "pico/stdlib.h"   // for sleep_ms()
#include "queue.h"         // request_queue_t + queue_pop()
#include "http_server.h"   // handle_http_request()
#include "core1.h"         // declares core1_main()

void core1_main(void) {
    request_t req;
    while (true) {
        if (queue_pop(&request_queue, &req)) {
            handle_http_request(req.pcb, req.buffer, req.length);
            // NOTE: we do NOT tcp_close() here or in the sent-callback
        } else {
            sleep_ms(1);
        }
    }
}
