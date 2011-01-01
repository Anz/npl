#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stddef.h>
#include "container.h"

#define ASM_DATA_SEGMENT "data"
#define ASM_TEXT_SEGMENT "text"
#define ASM_COMMENT '#'
#define ASM_SEGMENT '.'
#define ASM_TAB '\t'
#define ASM_SPACE ' '

void assembler(FILE* input, FILE* output);

#endif // ASSEMBLER_H_INCLUDED
