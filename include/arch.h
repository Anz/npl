#ifndef ARCH_H
#define ARCH_H

#include "container.h"
#include "library.h"

typedef struct arch_native {
    void (*main)();
    void* text;
    size_t text_size;
    void* data;
} arch_native_t;

arch_native_t arch_compile(ctr_t* container, library_t* library);

#endif

