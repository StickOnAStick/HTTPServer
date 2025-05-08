
#include <pthread.h>
#include <stdio.h>
#include "queue.h"
#include "task.h"

void* worker_thread(void* arg) {
    /*
        The actual worker threads we create. 

        Recieves a copy of the queue. 
        Each task is a unique function and arguments this thread is responsible for.
    */
    queue_t* queue = (queue_t*)arg;

    while(1) {
        task_t task = queue_pop(queue);
        task.function(task.arg);
    }
    return NULL;
}