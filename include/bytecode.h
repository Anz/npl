#ifndef BYTECODE_H
#define BYTECODE_H

#define BC_BYTECODE_VERSION 1
#define BC_OPCODE_SIZE 5

#define BC_NOP    0x00
#define BC_ARG    0x01
#define BC_SYNC   0x02
#define BC_ASYNC  0x03
#define BC_RET    0x04
#define BC_CMP    0x05
#define BC_JMP    0x06
#define BC_JE     0x07
#define BC_JNE    0x08
#define BC_JL     0x09
#define BC_JLE    0x0A
#define BC_JG     0x0B
#define BC_JGE    0x0C
#define BC_ENTER  0x0D
#define BC_SYNCE  0x0E
#define BC_ASYNCE 0x0F
#define BC_INIT   0x10
#define BC_ARGV   0x11

#define BC_ASM_NOP    "nop"
#define BC_ASM_ARG    "arg"
#define BC_ASM_SYNC   "sync"
#define BC_ASM_ASYNC  "async"
#define BC_ASM_RET    "ret"
#define BC_ASM_CMP    "cmp"
#define BC_ASM_JMP    "jmp"
#define BC_ASM_JE     "je"
#define BC_ASM_JNE    "jne"
#define BC_ASM_JL     "jl"
#define BC_ASM_JLE    "jle"
#define BC_ASM_JG     "jg"
#define BC_ASM_JGE    "jge"
#define BC_ASM_ENTER  "enter"
#define BC_ASM_SYNCE  "synce"
#define BC_ASM_ASYNCE "asynce"
#define BC_ASM_INIT   "init"
#define BC_ASM_ARGV   "argv"

char bc_asm2op(char* assembly);
char* bc_op2asm(char opcode);

#endif // BYTECODE_H
