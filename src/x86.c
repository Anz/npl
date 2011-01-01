#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <math.h>

void arch_print() {
    /*static int i = 0;

    int sum = 2;
    for (int i = 0; i < 9999999; i++) {
        sum += pow(sum, 2);
    }

    i++;*/
    printf("moins asfd\n");
}

void* call_addr(void* source, void* target) {
    if (source > target) {
        return (void*)0xFFFFFFFF - (source - target);
    } else {
        return (void*)(target - source);
    }
}

arch_native_t arch_compile(ctr_segment_t symbol, ctr_segment_t data, ctr_segment_t text) {
    arch_native_t native;

    size_t text_size = text.size + symbol.size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(text_size);

    char* ptr = native.text;

    unsigned int enter = swap_endian(0xC8040000);
    memcpy(ptr, &enter, 4);
    ptr+=4;

    *ptr = 0xE8;
    ptr++;
    void* addr = call_addr(ptr+4, arch_print);
    memcpy(ptr, &addr, 4);
    ptr+=4;
    *ptr = 0x90;
    ptr++;
    *ptr = 0x90;
    ptr++;
    *ptr = 0x90;
    ptr++;

    unsigned int leave = swap_endian(0xC9C39090);
    memcpy(ptr, &leave, 4);
    ptr+=4;

    native.main = native.text;

    return native;
}
