
#include "queue.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

void queue_init(queue_t* q){
    q->front = q->rear = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->sig, NULL);
}

void queue_push(queue_t* q, task_t task){
    pthread_mutex_lock(&q->lock);

    if(q->count == QUEUE_CAPACITY){
        // THROW 504 error
        fprintf(stderr, "Queue full!");
        exit(EXIT_FAILURE);
    }

    q->tasks[q->rear] = task;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;

    pthread_cond_signal(&q->sig);
    pthread_mutex_unlock(&q->lock);
}

task_t queue_pop(queue_t* q){

    pthread_mutex_lock(&q->lock);

    while(q->count == 0){
        pthread_cond_wait(&q->sig, &q->lock);
    }

    task_t res_task = q->tasks[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    
    pthread_mutex_unlock(&q->lock);
    return res_task;
}