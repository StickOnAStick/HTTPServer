#include "queue.h"
#include "pico/mutex.h"

request_queue_t request_queue; // Global queue instance defined in queue.h

void queue_init(request_queue_t* queue){
    mutex_init(&queue->lock);
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}

bool queue_push(request_queue_t* queue, request_t* req){
    bool res = false;
    if(!mutex_enter_block_until(&queue->lock, get_absolute_time() + 5*1000000)) // Wait 5 seconds
        return res;
    
    if(queue->count < REUQEST_QUEUE_SIZE){
        queue->requests[queue->tail] = *req;
        queue->tail = (queue->tail + 1) % REUQEST_QUEUE_SIZE;
        queue->count++;
        res = true;
    }

    mutex_exit(&queue->lock);
    return res;
}


bool queue_pop(request_queue_t *queue, request_t* out_req){
    bool res = false;
    if(!mutex_enter_block_until(&queue->lock, get_absolute_time() + 5*1000000))
        return res;
    
    if(queue->count > 0){
        *out_req = queue->requests[queue->head];
        queue->head = (queue->head + 1) % REUQEST_QUEUE_SIZE;
        queue->count--;
        res = true;
    }
    mutex_exit(&queue->lock);
    return res;
}

