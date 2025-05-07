#ifndef TASK_H
#define TASK_H

typedef struct {
    void (*function)(void* arg);
    void *arg;
} task_t;

#endif