#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#define ASM_ARG  0x00
#define ASM_INIT 0x01
#define ASM_CALL 0x02
#define ASM_CMP  0x03

void asm_init();
void asm_release();
char asm_mnemonic2opcode(char* mnemonic);
char* asm_opcode2mnemonic(char opcode);

#endif
