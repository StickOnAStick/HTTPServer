// inc/queue.h
#pragma once

#include <stdint.h>
#include "pico/mutex.h"
#include "lwip/tcp.h"

#define REQUEST_BUFFER_SIZE 1024
#define REQUEST_QUEUE_SIZE    8

typedef struct {
    struct tcp_pcb *pcb;
    char            buffer[REQUEST_BUFFER_SIZE];
    size_t          length;
} request_t;

typedef struct {
    request_t requests[REQUEST_QUEUE_SIZE];
    uint8_t   head, tail, count;
    mutex_t   lock;
} request_queue_t;

// global instance
extern request_queue_t request_queue;

// API
void queue_init(request_queue_t *q);
bool queue_push(request_queue_t *q, const request_t *r);
bool queue_pop (request_queue_t *q, request_t *out);
