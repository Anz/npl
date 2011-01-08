#include "bytecode.h"
#include <stdlib.h>
#include <string.h>

char bc_asm2op(char* assembly) {

    if (strcmp(BC_ASM_NOP,assembly) == 0) {
        return BC_NOP;
    }

    if (strcmp(BC_ASM_ARG,assembly) == 0) {
        return BC_ARG;
    }

    if (strcmp(BC_ASM_SYNC,assembly) == 0) {
        return BC_SYNC;
    }

    if (strcmp(BC_ASM_ASYNC,assembly) == 0) {
        return BC_ASYNC;
    }

    if (strcmp(BC_ASM_RET,assembly) == 0) {
        return BC_RET;
    }

    if (strcmp(BC_ASM_CMP,assembly) == 0) {
        return BC_CMP;
    }

    if (strcmp(BC_ASM_JMP,assembly) == 0) {
        return BC_JMP;
    }

    if (strcmp(BC_ASM_JE,assembly) == 0) {
        return BC_JE;
    }

    if (strcmp(BC_ASM_JNE,assembly) == 0) {
        return BC_JNE;
    }

    if (strcmp(BC_ASM_JL,assembly) == 0) {
        return BC_JL;
    }

    if (strcmp(BC_ASM_JLE,assembly) == 0) {
        return BC_JLE;
    }

    if (strcmp(BC_ASM_JG,assembly) == 0) {
        return BC_JG;
    }

    if (strcmp(BC_ASM_JGE,assembly) == 0) {
        return BC_JGE;
    }

    if (strcmp(BC_ASM_ENTER,assembly) == 0) {
        return BC_ENTER;
    }

    if (strcmp(BC_ASM_SYNCE,assembly) == 0) {
        return BC_SYNCE;
    }

    if (strcmp(BC_ASM_ASYNCE,assembly) == 0) {
        return BC_ASYNCE;
    }

    if (strcmp(BC_ASM_ARGV,assembly) == 0) {
        return BC_ARGV;
    }

    return -1;
}

char* bc_op2asm(char opcode) {
    switch(opcode) {
        case BC_NOP: return BC_ASM_NOP;
        case BC_ARG: return BC_ASM_ARG;
        case BC_SYNC: return BC_ASM_SYNC;
        case BC_ASYNC: return BC_ASM_ASYNC;
        case BC_RET: return BC_ASM_RET;
        case BC_CMP: return BC_ASM_CMP;
        case BC_JMP: return BC_ASM_JMP;
        case BC_JE: return BC_ASM_JE;
        case BC_JNE: return BC_ASM_JNE;
        case BC_JL: return BC_ASM_JL;
        case BC_JLE: return BC_ASM_JLE;
        case BC_JG: return BC_ASM_JG;
        case BC_JGE: return BC_ASM_JGE;
        case BC_ENTER: return BC_ASM_ENTER;
        case BC_SYNCE: return BC_ASM_SYNCE;
        case BC_ASYNCE: return BC_ASM_ASYNCE;
        case BC_ARGV: return BC_ASM_ARGV;
        default: return NULL;
    }
}
