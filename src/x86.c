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

    // read symbols
    ctr_symbol_t* symbols = ctr_symbol_read(input, header);

    // jump to text segment
    long text_offset = CTR_HEADER_SIZE + header.symbol_segment_size + header.data_segment_size;
    fseek(input, text_offset, SEEK_SET);

    // alloc space
    size_t text_size = header.text_segment_size + header.symbol_segment_size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(text_size);
    native.main = native.text;

    long index = 0;

    // compile to x86 code
    char opcode[BC_OPCODE_SIZE];
    char* instruction = opcode;
    unsigned int* arg = (unsigned int*)&opcode[1];
    int count = header.text_segment_size / BC_OPCODE_SIZE;
    for(int i = 0; i < count; i++) {
        if (ctr_symbol_find(symbols, header, i*BC_OPCODE_SIZE) >= 0) {
            if (i > 0) {
                write1(X86_LEAVE, native.text, &index);
                write1(X86_RET, native.text, &index);
            }
            write4(X86_ENTER, native.text, &index);
        }

        fread(opcode, sizeof(char), sizeof(opcode), input);
        switch(*instruction) {
            case BC_NOP: {
                write1(X86_NOP, native.text, &index);
                break;
            }
            case BC_SYNC: {
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
    write1(X86_LEAVE, native.text, &index);
    write1(X86_RET, native.text, &index);

    // release symbol
    free(symbols);

    char* text = native.text;
    for (int i = 0; i < 40; i+=4) {
        for (int j = 0; j < 4; j++) {
            unsigned int value = text[i+j];
            value = (value << 24) >> 24;
            printf("%02X ", value);
        }
        printf("\n");
    }

    return native;
}
