#include "core1.h"
#include "queue.h"
#include "http_server.h"

void core1_main() {
    request_t req;
    while (1) {
        if (queue_pop(&request_queue, &req)) {
            handle_http_request(req.pcb, req.buffer, req.length);
        } else {
            sleep_ms(1);
        }
    }
}