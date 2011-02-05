#ifndef THREAD_H
#define THREAD_H

#include "list.h"
#include <pthread.h>

typedef struct job {
    void (*func)(int argc, void** argv);
    int argc;
    void** argv;
} job_t;

typedef struct thread {
    pthread_t thread;
    pthread_attr_t attr;
    void* status;
} thread_t;

void threads_init();
void threads_release();
void threads_push(void (*func)(int, void**), int argc, void** argv);
void thread_init(thread_t* thread);
void thread_join(thread_t* thread);

#endif
