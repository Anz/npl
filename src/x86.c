#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "bytecode.h"

// x86 opcdoes
#define X86_ENTER 0xC8
#define X86_CALL 0xE8
#define X86_NOP 0x90
#define X86_LEAVE 0xC9
#define X86_RET 0xC3
#define X86_REGISTER 0x83
#define X86_SUB_ESP 0xEC
#define X86_PUSH 0xFF

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

arch_native_t arch_compile(ctr_header_t header, FILE* input, map_t* library) {
    arch_native_t native;

    // get external symbol table
    unsigned int external_count = ctr_external_count(header);
    void* externals[external_count];
    for (unsigned int i = 0; i < external_count; i++) {
        ctr_external_symbol_t external = ctr_external_read(input, header, i);
        map_node_t* function = map_find(library, external.name);
        if (function == NULL) {
            fprintf(stderr, "function not found: %s\n", external.name);
            externals[i] = 0;
            continue;
        }
        memcpy(&externals[i], function->value, sizeof(int));
    }

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
                write1(X86_ENTER, native.text, &index);
                write1(0, native.text, &index);
                write1(0, native.text, &index);
                write1(0, native.text, &index);
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
            case BC_INIT: {
                write1(X86_REGISTER, native.text, &index);
                write1(X86_SUB_ESP, native.text, &index);
                write1(4, native.text, &index);
                break;
            }
            case BC_ARG: {
                write1(X86_PUSH, native.text, &index);
                write1(0x75, native.text, &index);
                write1(0xFC, native.text, &index);
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                if (bc.argument < 0 || bc.argument >= external_count) {
                    fprintf(stderr, "external symbol not fount: %i\n", bc.argument);
                    continue;
                }
                void* function = externals[bc.argument];
                write1(X86_CALL, native.text, &index);
                void* addr = call_addr(native.text + index + 3, function);
                write4((unsigned int)addr, native.text, &index);
                break;
           }
           default: {
                write1(X86_NOP, native.text, &index);
                break;
            }
        }
    }


    return native;
}
