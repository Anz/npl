#include <string.h>
#include "assembly.h"
#include "bytecode.h"

#define READ_BUFFER_SIZE 1024
#define SEGMENT_BUFFER_SIZE 1024

int isletter(char c) {
    return 'a' <= c && c <= 'z';
}

void jump_to_text_seg(FILE* file) {
    char line[READ_BUFFER_SIZE];
    memset(line, 0, READ_BUFFER_SIZE);
    while(!feof(file) && memcmp(line, ".text", 5) != 0)  {
        fgets(line, READ_BUFFER_SIZE, file);
    }
}

size_t min_s(size_t a, size_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

void add_symbol(ctr_segment_t* symbol_segment, char* line, ctr_addr addr) {
    static unsigned int symbol_index = 0;

    ctr_symbol_resize_segment(symbol_segment, symbol_index+1);

    ctr_symbol_set_addr(*symbol_segment, symbol_index, addr);
    char* name = strtok(line, ": ");
    ctr_symbol_set_name(*symbol_segment, symbol_index, name);

    printf("<%s> <%08X>\n", name, addr);

    symbol_index++;
}

void assembler(FILE* file, ctr_header_t* header, ctr_segment_t* symbol_segment, ctr_segment_t* data_segment, ctr_segment_t* text_segment) {
    // set settings
    unsigned int text_index = 0;

    symbol_segment->size = 0;
    symbol_segment->data = NULL;
    data_segment->size = 0;
    data_segment->data = NULL;
    text_segment->size = 0;
    text_segment->data = NULL;

    // jump to text segment
    jump_to_text_seg(file);

    char line[READ_BUFFER_SIZE];
    while(!feof(file)) {
        fgets(line, READ_BUFFER_SIZE, file);

        switch(line[0]) {
            // ignore comment
            case ASM_COMMENT: break;
            // abort on new segment
            case ASM_SEGMENT: return;
            // on instruction
            case ASM_SPACE:
            case ASM_TAB: {
                char* mnemonic = strtok(line, "\t ");
                char instruction = bc_asm2op(mnemonic);
                ctr_text_resize_segment(text_segment, text_index+1);
                ctr_text_set_instruction(*text_segment, text_index, instruction);
                text_index++;
            }

            default: {
                // on function
                if (isletter(line[0])) {
                    add_symbol(symbol_segment, line, text_index * BC_OPCODE_SIZE);
               }
            }
        }
    }
}
