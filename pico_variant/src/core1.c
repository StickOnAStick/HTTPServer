#include "core1.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "types.h"

void core1_main(){
    request_t req;

    while(true){
        if(queue_pop(&request_queue, &req)){
            handle_http_request(req.pcb, req.buffer, req.length);
        }else {
            sleep_ms(1);
        }
    }
}