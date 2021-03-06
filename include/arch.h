#ifndef ARCH_H
#define ARCH_H

#include "container.h"
#include "lib/library.h"

typedef struct arch_native {
    void (*main)(int, void**);
    void* text;
    size_t text_size;
    void* data;
} arch_native_t;

arch_native_t arch_compile(map_t* symbols, library_t* library);

#endif

