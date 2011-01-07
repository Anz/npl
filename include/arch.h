#ifndef ARCH_H
#define ARCH_H

#include "container.h"
#include "map.h"

typedef struct arch_native {
    void (*main)();
    void* text;
    void* data;
} arch_native_t;

arch_native_t arch_compile(ctr_header_t header, FILE* input, map_t* library);

#endif

