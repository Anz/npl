#ifndef ARCH_H
#define ARCH_H

#include "container.h"

typedef struct arch_native {
    void (*main)();
    void* text;
    void* data;
} arch_native_t;

arch_native_t arch_compile(ctr_segment_t symbol, ctr_segment_t data, ctr_segment_t text);

#endif

