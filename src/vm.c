#include <stdlib.h>
#include <stdio.h>
#include "container.h"
#include "arch.h"
#include "job.h"
#include "thread.h"
#include <pthread.h>

int main(int argc, char* argv[]) {

    printf("start\n");

    // parse args
    if (argc < 2) {
        printf("usage: %s <module>\n", argv[0]);
        return 1;
    }

    char* path;
    int thread_count = 1;
    if (argc > 2) {
        thread_count = atoi(argv[1]);
    }
    path = argv[argc-1];

    // open module
    FILE* module = fopen(path, "r");
    if (module == NULL) {
        printf("cannot open: %s\n", argv[1]);
        return 2; 
    }

    // read module
    ctr_header_t header = ctr_read_header(module);
    ctr_segment_t symbol = ctr_read_symbol_segment(module, header);
    ctr_segment_t data = ctr_read_data_segment(module, header);
    ctr_segment_t text = ctr_read_text_segment(module, header);

    // compile bytecode
    arch_native_t native = arch_compile(symbol, data, text);

    // create main job
    job_t main_job;
    main_job.func = native.main;
    main_job.args = NULL;
    main_job.size = 0;

    // init
    job_list_t jobs;
    job_list_init(&jobs);
    for (int i = 0; i < 10; i++) {
        job_list_push(&jobs, main_job);
     }

    thread_t threads[thread_count];

    // create threads
    /*for (int i = 0; i < thread_count; i++) {
        thread_init(&threads[i], &jobs);
    }
    // wait for threads
    for (int i = 0; i < thread_count; i++) {
        thread_join(&threads[i]);
    }*/
    printf("%08X: %08X\n", native.main, *(unsigned int*)native.main);
    native.main();

    // releasep
    job_list_release(&jobs);

    printf("end\n");

    pthread_exit(NULL);

    return 0;
}
