#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "bytecode.h"

// x86 opcdoes
#define X86_ENTER swap_endian(0xC8040000)
#define X86_CALL 0xE8
#define X86_NOP 0x90
#define X86_LEAVE 0xC9
#define X86_RET 0xC3

void arch_print() {
    printf("moins asfd\n");
}

void* call_addr(void* source, void* target) {
    if (source > target) {
        return (void*)0xFFFFFFFF - (source - target);
    } else {
        return (void*)(target - source);
    }
}

void write1(char data, char* buffer, long* index) {
    buffer[*index] = data;
    *index += 1;
}

void write4(unsigned int data, char* buffer, long* index) {
    memcpy(&buffer[*index], &data, 4);
    *index += 4; 
}

arch_native_t arch_compile(ctr_header_t header, FILE* input) {
    arch_native_t native;

    // alloc space
    size_t text_size = header.text_segment_size + header.symbol_segment_size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(text_size);
    native.main = native.text;

    long index = 0;

    // compile to x86 code
    unsigned int count = ctr_text_count(header);
    for(unsigned int i = 0; i < count; i++) {
        ctr_bytecode_t bc = ctr_text_read(input, header, i);
        switch(bc.instruction) {
            case BC_ENTER: {
                write4(X86_ENTER, native.text, &index);
                break;
            }
            case BC_RET: {
                write1(X86_LEAVE, native.text, &index);
                write1(X86_RET, native.text, &index);
                break;
            }
            case BC_NOP: {
                write1(X86_NOP, native.text, &index);
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                write1(X86_CALL, native.text, &index);
                void* addr = call_addr(native.text + index + 4, arch_print);
                write4((unsigned int)addr, native.text, &index);
                break;
           }
           default: {
                write1(X86_NOP, native.text, &index);
                break;
            }
        }
    }

    char* text = native.text;
    for (int i = 0; text[i] != 0;) {
        int size = 1;
        for (int j = 0; j < size; j++) {
            if (text[i+j] == (char)X86_CALL) {
                size = 5;
            } else if (text[i+j] == (char)0xC8) {
                size = 4;
            } else if (text[i+j] == (char)0xC9) {
                size = 2;
            }
            unsigned int value = text[i+j];
            value = (value << 24) >> 24;
            printf("%02X ", value);
        }
        i += size;
        printf("\n");
    }

    return native;
}
