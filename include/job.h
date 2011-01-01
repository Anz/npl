#ifndef JOB_H
#define JOB_H

#include <stddef.h>
#include <pthread.h>

typedef struct job {
    void (*func)();
    char* args;
    size_t size;
} job_t;

typedef struct job_list {
    job_t* jobs;
    unsigned int count;
    pthread_mutex_t mutex;
} job_list_t;


void job_list_init(job_list_t* list);
void job_list_release(job_list_t* list);
void job_list_push(job_list_t* list, job_t job);
job_t job_list_pop(job_list_t* list);
void job_list_lock(job_list_t* list);
void job_list_unlock(job_list_t* list);

#endif
