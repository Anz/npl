#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stddef.h>
#include <nof.h>

#define ASM_DATA_SEGMENT "data"
#define ASM_TEXT_SEGMENT "text"
#define ASM_COMMENT '#'
#define ASM_SEGMENT '.'
#define ASM_INSTRUCTION '\t'

typedef struct asm_info {
    size_t data_section;
    size_t text_section;
    unsigned int symbol_count;
    size_t symbol_segment_size;
    unsigned int instruction_count;
    size_t text_segment_size;
} asm_info_t;

asm_info_t asm_collection_info(FILE* file);
void asm_fill_segment(FILE* file, char* symbol_segment, char* text_segment, asm_info_t info);


#endif // ASSEMBLER_H_INCLUDED
