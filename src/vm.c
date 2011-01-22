#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "container.h"
#include "arch.h"
#include "job.h"
#include "thread.h"
#include "map.h"
#include "integer.h"

void print_arch_code(void* address, size_t seg_size);

void print_wtf() {
    printf("hello vm!\n");
}

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
    FILE* module = fopen(path, "rb");
    if (module == NULL) {
        printf("cannot open: %s\n", argv[1]);
        return 2; 
    }

    // read module
    ctr_header_t header = ctr_read_header(module);

    // compile bytecode
    map_t library;
    int faddr = (int)&print_wtf;
    map_init(&library, 0, sizeof(int));
    map_set(&library, "print", &faddr);
    //map_add(&library, "integer", &faddr);
    library_add_integer(&library);
    //map_add(&library, "integer_set", &faddr);
    arch_native_t native = arch_compile(header, module, &library);
    fclose(module);
    print_arch_code(native.main, native.text_size);

    // create main job
    job_t main_job;
    main_job.func = native.main;
    main_job.args = NULL;
    main_job.size = 0;

    // init
    job_list_t jobs;
    job_list_init(&jobs);
    /*for (int i = 0; i < 1; i++) {
        job_list_push(&jobs, main_job);
     }

    thread_t threads[thread_count];

    // create threads
    for (int i = 0; i < thread_count; i++) {
        thread_init(&threads[i], &jobs);
    }
    // wait for threads
    for (int i = 0; i < thread_count; i++) {
        thread_join(&threads[i]);
    }*/
    native.main();
    

    // releasep
    job_list_release(&jobs);
    free(native.text);

    printf("end\n");

    pthread_exit(NULL);

    return 0;
}

void print_arch_code(void* address, size_t seg_size) {
    char* text = (char*)address;
    for (int i = 0; i < seg_size;) {
        int size = 1;
        for (int j = 0; j < size; j++) {
            if (j == 0) {
                switch (text[i+j]) {
                    case (char)0x68:
                    case (char)0x55:
                    case (char)0xE8: size = 5; break;

                    case (char)0xC8: size = 4; break;

                    case (char)0x83: 
                    case (char)0xFF: size = 3; break;

                    case (char)0xC9: size = 2; break;
                }
            }
            unsigned int value = text[i+j];
            value = (value << 24) >> 24;
            printf("%02X ", value);
        }
        i += size;
        printf("\n");
    }
}
