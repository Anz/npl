#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#define ASM_NOP    0x00
#define ASM_ENTER  0x01
#define ASM_ARG    0x02
#define ASM_ARGV   0x03
#define ASM_CALL   0x04
#define ASM_CALLE  0x05
#define ASM_CMP    0x06
#define ASM_JMP    0x07
#define ASM_JE     0x08
#define ASM_JNE    0x09
#define ASM_JL     0x0A
#define ASM_JLE    0x0B
#define ASM_JG     0x0C
#define ASM_JGE    0x0D
#define ASM_RET    0x0E
#define ASM_INIT   0x0F

void asm_init();
void asm_release();
char asm_mnemonic2opcode(char* mnemonic);
char* asm_opcode2mnemonic(char opcode);

#endif
