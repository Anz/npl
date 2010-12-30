#include "thread.h"

void* thread_main(void* args) {
    thread_t* thread = args;
    
    while (thread->jobs->count > 0) {
        // get next job
        job_t job = job_list_pop(thread->jobs);

        // execite job
        job.func();
    }

    pthread_exit(NULL);
}

void thread_init(thread_t* thread, job_list_t* jobs) {
    thread->jobs = jobs;
    pthread_attr_init(&thread->attr);
    pthread_attr_setdetachstate(&thread->attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread->thread, &thread->attr, thread_main, thread);
}

void thread_join(thread_t* thread) {
    pthread_join(thread->thread, &thread->status);
}
