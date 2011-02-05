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

static char* write(char* buffer, void* data, size_t size) {
    memcpy(buffer, data, size);
    return buffer + size;
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

    int argument_count = 0;
    char* buffer = native.text;

    // compile to x86 code
    for(unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);
        switch(bc->instruction) {
            case ASM_ENTER: {
                char enter[] = { X86_ENTER, (char)bc->argument, 0, 0 };
                // enter <bc->argument, $0 (alloc space on stack for local vars)
                buffer = write(buffer, enter, sizeof(enter));
                break;
            }
            case ASM_RET: {
                char ret[] = { X86_LEAVE, X86_RET };
                // leave (clean up stack)
                // ret (return)
                buffer = write(buffer, ret, sizeof(ret));
                break;
            }
            case ASM_NOP: {
                char nop[] = { X86_NOP };
                // not a operation
                buffer = write(buffer, nop, sizeof(nop));
                break;
            }
            case ASM_ARG: {
                char size = (char)((bc->argument + 1) * 4);
                char arg[] = { 0x55, 0x83, 0x2C, 0x24, size };
                //  push value from stack
                buffer = write(buffer, arg, sizeof(arg));
                argument_count++;
                break;
            }
            case ASM_ARGV: {
                char* ptr = (char*)&bc->argument;
                // push <bc->argument>
                char argv[] = { X86_PUSH_VALUE, ptr[0], ptr[1], ptr[2], ptr[3] };
                buffer = write(buffer, argv, sizeof(argv));
                argument_count++;
                break;
            }
            case ASM_SYNC:
            case ASM_ASYNC: {
                void* subroutine = get_subroutine_addr(texts, buffer, i, i + bc->argument); 
                void* addr = (char*)call_addr(buffer + 4, subroutine);
                char* ptr = (char*)&addr;

                // call <addr> (call function)
                char sync[] = { X86_CALL, ptr[0], ptr[1], ptr[2], ptr[3] };
                buffer = write(buffer, sync, sizeof(sync));
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
                
                void* addr = call_addr(buffer + 4, function);
                char* ptr = (char*)&addr;
                char size = 4 * argument_count;
                
                // call <addr> (call function)
                // add esp, <size> (remove args from stack)
                char synce[] = { X86_CALL, ptr[0], ptr[1], ptr[2], ptr[3], X86_REGISTER, X86_ADD_ESP, size };
                buffer = write(buffer, synce, sizeof(synce));

                // reset argument count
                argument_count = 0;
                break;
           }
           default: {
                char nop[] = { X86_NOP };
                buffer = write(buffer, nop, sizeof(nop));
                break;
            }
        }
    }

    // release unused memory
    native.text_size = (void*)buffer - native.text;
    native.text = realloc(native.text, native.text_size);
    native.main = native.text;

    return native;
}
