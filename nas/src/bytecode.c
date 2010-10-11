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
        default: return NULL;
    }
}
