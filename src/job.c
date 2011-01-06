#include <stddef.h>
#include <stdlib.h>
#include "job.h"

void job_list_init(job_list_t* list) {
    list->jobs = NULL;
    list->count = 0;
    pthread_mutex_init(&list->mutex, NULL);
}

void job_list_release(job_list_t* list) {
    free(list->jobs);
    list->count = 0;
    pthread_mutex_destroy(&list->mutex);
}

void job_list_push(job_list_t* list, job_t job) {
    list->count++;
    list->jobs = realloc(list->jobs, list->count * sizeof(job_t));
    list->jobs[list->count-1] = job; 
}

job_t job_list_pop(job_list_t* list) {
    list->count--;
    return list->jobs[list->count];
}

void job_list_lock(job_list_t* list) {
    pthread_mutex_lock(&list->mutex);
}

void job_list_unlock(job_list_t* list) {
    pthread_mutex_unlock(&list->mutex);
}