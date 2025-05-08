#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "task.h"

#define QUEUE_CAPACITY 1024

typedef struct {
    task_t tasks[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;


    pthread_mutex_t lock; 
    // Lock for async communication

    pthread_cond_t  sig;  
    /*
        Thread pool signal. When items are inside the queue this signal is set, 
        notifying sleeping workers.
    
        Why? This prevents workers from having to contiually check the queue, wasting cycles.
    */
} queue_t;

void queue_init(queue_t* queue);
int queue_push(queue_t* queue, task_t task);
task_t queue_pop(queue_t* queue);

#endif