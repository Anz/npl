#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "container.h"
#include "arch.h"
#include "thread.h"
#include "map.h"
#include "lib/library.h"
#include <stdbool.h>
#include <string.h>

void print_arch_code(void* address, size_t seg_size);

int main(int argc, char* argv[]) {

    printf("start\n");

    // parse args
    if (argc < 2) {
        printf("usage: %s <module>\n", argv[0]);
        return 1;
    }

    bool show_native_code = false;
    int thread_count = 1;
    char* path = argv[argc-1];

    for (int i = 1; i < argc - 1; i++) {
        char* arg = argv[i];
        if (arg[0] != '-' || strlen(arg) < 2) {
            printf("usage: %s <module>\n", argv[0]);
        }
        switch (arg[1]) {
            case 's': show_native_code = true; break;
            case 't': thread_count = atoi(arg+2); break;
        }
    }

    // open module
    FILE* module = fopen(path, "rb");
    if (module == NULL) {
        printf("cannot open: %s\n", argv[1]);
        return 2; 
    }

    // read module
    ctr_t container = ctr_read(module);
    fclose(module);

    // compile bytecode
    library_t library;
    library_init(&library);
    arch_native_t native = arch_compile(&container, &library);

    // print native code
    if (show_native_code) {
        print_arch_code(native.main, native.text_size);
    }

    // create main job
    threads_init();
    threads_push(native.main, 0, NULL);

    thread_t threads[thread_count];

    // create threads
    for (int i = 0; i < thread_count; i++) {
        thread_init(&threads[i]);
    }
    // wait for threads
    for (int i = 0; i < thread_count; i++) {
        thread_join(&threads[i]);
    }

    // releasep
    threads_release();
    free(native.text);
    library_release(&library);

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
                    case (char)0xE9:
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
