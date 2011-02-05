#include "thread.h"

static pthread_mutex_t mutex;
static list_t jobs;

void threads_init() {
    list_init(&jobs, sizeof(job_t));
    pthread_mutex_init(&mutex, NULL);
}

void threads_release() {
    list_release(&jobs);
    pthread_mutex_destroy(&mutex);
}

void threads_push(void (*func)(int, void**), int argc, void** argv) {
    job_t job;
    job.func = func;
    job.argc = argc;
    job.argv = argv;
    pthread_mutex_lock(&mutex);
    list_add(&jobs, &job);
    pthread_mutex_unlock(&mutex);
}

static void* thread_main(void* args) {
    thread_t* thread = args;

    // get job
    job_t job;
    pthread_mutex_lock(&mutex);
    int count = list_pop(&jobs, &job);
    pthread_mutex_unlock(&mutex);

    while (count) {
        // execite job
        job.func(0, NULL);

        // get job
        pthread_mutex_lock(&mutex);
        count = list_pop(&jobs, &job);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void thread_init(thread_t* thread) {
    pthread_attr_init(&thread->attr);
    pthread_attr_setdetachstate(&thread->attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread->thread, &thread->attr, thread_main, thread);
}

void thread_join(thread_t* thread) {
    pthread_join(thread->thread, &thread->status);
}
