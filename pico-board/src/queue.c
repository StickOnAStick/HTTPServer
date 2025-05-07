// src/queue.c
#include "queue.h"
#include "pico/mutex.h"
#include "pico/time.h"    // for get_absolute_time()

// define the global
request_queue_t request_queue;

void queue_init(request_queue_t *q){
    mutex_init(&q->lock);
    q->head = q->tail = q->count = 0;
}

bool queue_push(request_queue_t *q, const request_t *r){
    bool ok = mutex_enter_block_until(&q->lock, get_absolute_time() + 5*1000000);
    if (!ok) return false;
    if (q->count < REQUEST_QUEUE_SIZE) {
        q->requests[q->tail] = *r;
        q->tail = (q->tail + 1) % REQUEST_QUEUE_SIZE;
        q->count++;
        ok = true;
    }
    mutex_exit(&q->lock);
    return ok;
}

bool queue_pop(request_queue_t *q, request_t *out){
    bool ok = mutex_enter_block_until(&q->lock, get_absolute_time() + 5*1000000);
    if (!ok) return false;
    if (q->count > 0) {
        *out = q->requests[q->head];
        q->head = (q->head + 1) % REQUEST_QUEUE_SIZE;
        q->count--;
        ok = true;
    }
    mutex_exit(&q->lock);
    return ok;
}
