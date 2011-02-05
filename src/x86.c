#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "assembly.h"

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

void* get_subroutine_addr(list_t* text, void* addr, int current,  int target) {
    size_t size = 0;
    int incrementor;
    if (current < target) {
        incrementor = 1;
        target--;
    } else {
        incrementor = -1;
        current--;
        size = 3;
    }
    for (int i = current; i != target; i += incrementor) {
        ctr_bytecode_t* bc = list_get(text, i);
        switch(bc->instruction) {
            case ASM_NOP:    size += 1; break;
            case ASM_ENTER:  size += 4; break;
            case ASM_ARG:    size += 5; break;
            case ASM_ARGV:   size += 5; break;
            case ASM_SYNC:   size += 5; break;
            case ASM_ASYNC:  size += 5; break;
            case ASM_SYNCE:  size += 8; break;
            case ASM_ASYNCE: size += 8; break;
            case ASM_CMP:    size += 1; break;
            case ASM_JMP:    size += 1; break;
            case ASM_JE:     size += 1; break;
            case ASM_JNE:    size += 1; break;
            case ASM_JL:     size += 1; break;
            case ASM_JLE:    size += 1; break;
            case ASM_JG:     size += 1; break;
            case ASM_JGE:    size += 1; break;
            case ASM_RET:    size += 2; break;
            default: size += 1; break;
        }
    }
    return addr + incrementor * (size + 1);
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
    native.text_size = header->text_size / CTR_BYTECODE_SIZE * 5 + header->symbol_size / CTR_SYMBOL_SIZE * 8;
    native.text = malloc(native.text_size);

    long index = 0;
    int argument_count = 0;

    // compile to x86 code
    for(unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);
        switch(bc->instruction) {
            case ASM_ENTER: {
                write1(X86_ENTER, native.text, &index);
                write1(bc->argument, native.text, &index);
                write1(0, native.text, &index);
                write1(0, native.text, &index);
                break;
            }
            case ASM_RET: {
                write1(X86_LEAVE, native.text, &index);
                write1(X86_RET, native.text, &index);
                break;
            }
            case ASM_NOP: {
                write1(X86_NOP, native.text, &index);
                break;
            }
            case ASM_ARG: {
                write1(0x55, native.text, &index); // push ebx
                write1(0x83, native.text, &index); // sub (argument+1)*4, (%esp)
                write1(0x2C, native.text, &index);
                write1(0x24, native.text, &index);
                char size = (char)((bc->argument + 1) * 4);
                write1(size, native.text, &index);
                argument_count++;
                break;
            }
            case ASM_ARGV: {
                write1(X86_PUSH_VALUE, native.text, &index);
                write4(bc->argument, native.text, &index);
                argument_count++;
                break;
            }
            case ASM_SYNC:
            case ASM_ASYNC: {
                void* subroutine = get_subroutine_addr(texts, native.text+index, i, i + bc->argument); 
                write1(X86_CALL, native.text, &index);
                void* addr = call_addr(native.text + index + 3, subroutine);
                write4((unsigned int)addr, native.text, &index);
              break;
            }
            case ASM_SYNCE:
            case ASM_ASYNCE: {
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
                //if (argument_count > 0) {
                    write1(X86_REGISTER, native.text, &index);
                    write1(X86_ADD_ESP, native.text, &index);
                    write1(4 * argument_count, native.text, &index);
                    argument_count = 0;
                //}
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
