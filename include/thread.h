#ifndef THREAD_H
#define THREAD_H

#include "job.h"
#include <pthread.h>

typedef struct thread {
    pthread_t thread;
    pthread_attr_t attr;
    void* status;
    job_list_t* jobs;
} thread_t;

void thread_init(thread_t* thread, job_list_t* jobs);
void thread_join(thread_t* thread);

#endif
