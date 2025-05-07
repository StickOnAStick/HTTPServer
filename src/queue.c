
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void queue_init(queue_t* q){
    q->front = q->rear = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->sig, NULL);
}

void queue_push(queue_t* q, task_t task){
    pthread_mutex_lock(&q->lock);
}