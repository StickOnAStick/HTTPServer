#pragma once

#include "types.h"

/*
    Defines class methods for the global queue (ring buffer)
*/

void queue_init(request_queue_t* queue);
bool queue_push(request_queue_t* queue, request_t* req);
bool queue_pop(request_queue_t* queue, request_t* out_req);