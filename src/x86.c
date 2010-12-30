#include "arch.h"
#include <stdio.h>

void arch_print() {
    printf("moins asfd\n");
}

arch_native_t arch_compile(ctr_segment_t symbol, ctr_segment_t data, ctr_segment_t text) {
    arch_native_t native;

    native.main = arch_print;

    return native;
}
