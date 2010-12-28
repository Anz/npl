#ifndef BYTECODE_H
#define BYTECODE_H

#define BC_BYTECODE_VERSION 1
#define BC_OPCODE_SIZE 5

#define BC_NOP   0x00
#define BC_ARG   0x01
#define BC_SYNC  0x02
#define BC_ASYNC 0x03
#define BC_RET   0x04
#define BC_CMP   0x05
#define BC_JMP   0x06

#define BC_ASM_NOP   "nop"
#define BC_ASM_ARG   "arg"
#define BC_ASM_SYNC  "sync"
#define BC_ASM_ASYNC "async"
#define BC_ASM_RET   "ret"
#define BC_ASM_CMP   "cmp"
#define BC_ASM_JMP   "jmp"

char bc_asm2op(char* assembly);
char* bc_op2asm(char opcode);

#endif // BYTECODE_H
