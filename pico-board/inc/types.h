#pragma once

#include "pico/mutex.h"
#include "lwip/tcp.h"

#define REQUEST_BUFFER_SIZE 1024
#define REUQEST_QUEUE_SIZE 8

typedef struct {
    struct tcp_pcb *pcb; // TCP connection
    char buffer[REQUEST_BUFFER_SIZE];
    size_t length; // Data length
} request_t;

typedef struct {
    request_t requests[REUQEST_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    mutex_t lock;
} request_queue_t;


// Global queue instance
extern request_queue_t request_queue;