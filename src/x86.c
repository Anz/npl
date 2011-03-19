#include "arch.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "util.h"
#include "assembly.h"
#include <sys/mman.h>
#include <errno.h>

// x86 opcdoes
#define X86_ENTER      0xC8
#define X86_CALL       0xE8
#define X86_JMP        0xE9
#define X86_NOP        0x90
#define X86_LEAVE      0xC9
#define X86_RET        0xC3
#define X86_REGISTER   0x83
#define X86_SUB_ESP    0xEC
#define X86_ADD_ESP    0xC4
#define X86_PUSH       0xFF
#define X86_PUSH_VALUE 0x68

#define PAGESIZE 4096

static void* call_addr(void* source, void* target) {
    if (source > target) {
        return (void*)0xFFFFFFFF - 4 + (target - source);
    }
    return (void*)(target - source) - 4;
}

static void* get_subroutine_addr(list_t* text, void* addr, int current,  int target) {
    /*size_t size = 0;
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
            /*case ASM_NOP:    size += 1; break;
            case ASM_ENTER:  size += 4; break;
            case ASM_ARG:    size += 5; break;
            case ASM_ARGV:   size += 5; break;
            case ASM_CALL:   size += 5; break;
            case ASM_CALLE:  size += 8; break;
            case ASM_CMP:    size += 1; break;
            case ASM_JMP:    size += 5; break;
            case ASM_JE:     size += 1; break;
            case ASM_JNE:    size += 1; break;
            case ASM_JL:     size += 1; break;
            case ASM_JLE:    size += 1; break;
            case ASM_JG:     size += 1; break;
            case ASM_JGE:    size += 1; break;
            case ASM_RET:    size += 2; break;*//*
            default: size += 1; break;
        }
    }
    return addr + incrementor * (size + 1);*/
    return NULL;
}

static char* write(char* buffer, int num, ...) {
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num; i++) {
        buffer[i] = (char)va_arg(args, int);
    }
    va_end(args);
    return buffer + num;
}

arch_native_t arch_compile(ctr_t* container,  library_t* library) {
    arch_native_t native;

/*    ctr_header_t* header = &container->header;
    map_t* symbols = &container->symbols;
    map_t* externals = &container->externals;
    list_t* texts = &container->texts;

    // alloc space
    native.text_size = (header->text_size / CTR_BYTECODE_SIZE * 5 + header->symbol_size / CTR_SYMBOL_SIZE * 8) + PAGESIZE - 1;
    native.text = malloc(native.text_size);
    char* buffer = (char*)(((int) native.text + PAGESIZE -1) & ~ (PAGESIZE-1));
    native.main = (void*)buffer;
    if (mprotect(buffer, 1024, PROT_READ | PROT_WRITE | PROT_EXEC) != 0 ) {
       fprintf(stderr, "failed to set memory executable: %s\n", strerror(errno));
    }

    list_t arg_stack;
    list_init(&arg_stack, sizeof(ctr_bytecode_t));

    // compile to x86 code
    for(unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);
        switch(bc->instruction) {
            case ASM_ENTER: {
                // enter <bc->argument, $0 (alloc space on stack for local vars)
                buffer = write(buffer, 4, X86_ENTER, bc->argument, 0, 0);
                break;
            }
            case ASM_RET: {
                // leave (clean up stack)
                // ret (return)
                buffer = write(buffer, 2, X86_LEAVE, X86_RET);
                break;
            }
            case ASM_NOP: {
                // not a operation
                buffer = write(buffer, 1, X86_NOP);
                break;
            }
            case ASM_ARG: 
            case ASM_ARGV: {
                list_add(&arg_stack, bc);
                break;
            }
            case ASM_CALL: {
                void* subroutine = get_subroutine_addr(texts, buffer, i, i + bc->argument); 
                void* addr = (char*)call_addr(buffer, subroutine);
                char* ptr = (char*)&addr;

                // call <addr> (call function)
                buffer = write(buffer, 5, X86_CALL, ptr[0], ptr[1], ptr[2], ptr[3]);

                break;
            }
            case ASM_CALLE: {
                for (int i = 0; i < arg_stack.count; i++) {
                    ctr_bytecode_t* bytecode  = list_get(&arg_stack, i);
                    switch(bytecode->instruction) {
                        case ASM_ARG: {
                            char size = (char)bytecode->argument;
                            //  push value from stack
                            buffer = write(buffer, 5, 0x55, 0x83, 0x2C, 0x24, size);
                            break;
                        }
                        case ASM_ARGV: {
                            char* ptr = (char*)&bytecode->argument;
                            // push <bc->argument>
                            buffer = write(buffer,5,  X86_PUSH_VALUE, ptr[0], ptr[1], ptr[2], ptr[3]);
                            break;
                        }
                   }
                }

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
                
                void* addr = call_addr(buffer, function);
                char* ptr = (char*)&addr;
                char size = 4 * arg_stack.count;
                
                // call <addr> (call function)
                // add esp, <size> (remove args from stack)
                buffer = write(buffer, 8, X86_CALL, ptr[0], ptr[1], ptr[2], ptr[3], X86_REGISTER, X86_ADD_ESP, size);

                list_clear(&arg_stack);
                break;
            }
            case ASM_JMP: {
                void* subroutine = get_subroutine_addr(texts, buffer, i, i + bc->argument); 
                void* addr = (char*)call_addr(buffer, subroutine) - 1;
                char* ptr = (char*)&addr;

                // call <addr> (call function)
                buffer = write(buffer, 5, X86_JMP, ptr[0], ptr[1], ptr[2], ptr[3]);
                 break;     
            }
            default: {
                buffer = write(buffer, 1, X86_NOP);
                break;
            }
        }
    }

    list_release(&arg_stack);*/

    return native;
}
