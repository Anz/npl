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
#define X86_ADD_ESP 0xC4
#define X86_PUSH 0xFF
#define X86_PUSH_VALUE 0x68

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

arch_native_t arch_compile(ctr_t* container,  library_t* library) {
    arch_native_t native;

    ctr_header_t* header = &container->header;
    map_t* symbols = &container->symbols;
    map_t* externals = &container->externals;
    list_t* texts = &container->texts;

    // alloc space
    native.text_size = header->text_size / BC_OPCODE_SIZE * 5 + header->symbol_size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(native.text_size);

    long index = 0;
    int argument_count = 0;

    // compile to x86 code
    for(unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);
        switch(bc->instruction) {
            case BC_ENTER: {
                write1(X86_ENTER, native.text, &index);
                write1(bc->argument, native.text, &index);
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
            case BC_ARG: {
                write1(0x55, native.text, &index); // push ebx
                write1(0x83, native.text, &index); // sub (argument+1)*4, (%esp)
                write1(0x2C, native.text, &index);
                write1(0x24, native.text, &index);
                char size = (char)((bc->argument + 1) * 4);
                write1(size, native.text, &index);
                argument_count++;
                break;
            }
            case BC_ARGV: {
                write1(X86_PUSH_VALUE, native.text, &index);
                write4(bc->argument, native.text, &index);
                argument_count++;
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                char* symbol = map_find_value(externals, &bc->argument);
                if (!symbol) {
                    fprintf(stderr, "external symbol not fount: %i\n", bc->argument);
                    continue;
                }
                void* function = library_get(library, symbol);
                if (!function) {
                    fprintf(stderr, "undefined reference to extneral symbol '%s'\n", symbol);
                    continue;
                }
                write1(X86_CALL, native.text, &index);
                void* addr = call_addr(native.text + index + 3, function);
                write4((unsigned int)addr, native.text, &index);

                // reset stack pointer
                if (argument_count > 0) {
                    write1(X86_REGISTER, native.text, &index);
                    write1(X86_ADD_ESP, native.text, &index);
                    write1(4 * argument_count, native.text, &index);
                    argument_count = 0;
                }
                break;
           }
           default: {
                write1(X86_NOP, native.text, &index);
                break;
            }
        }
    }

    // release unused memory
    native.text_size = index;
    native.text = realloc(native.text, native.text_size);
    native.main = native.text;

    return native;
}
