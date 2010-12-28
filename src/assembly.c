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

void add_symbol(nof_header_t* header, char** symbol_seg_ptr, char* line, ctr_addr addr) {
    static unsigned int symbol_index = 0;

    char* name = strtok(line, ": ");
    nof_symbol_set_addr(*symbol_seg_ptr, symbol_index, addr);
    nof_symbol_set_name(*symbol_seg_ptr, symbol_index, name);

    printf("<%s> <%08X>\n", name, addr);

    symbol_index++;
    header->symbol_segment_size += NOF_SYMBOL_SIZE;
}

void assembler(FILE* file, nof_header_t* header, char** symbol_seg_ptr, char** data_seg_ptr, char** text_seg_ptr) {
    ctr_addr text_ptr = 0;

    // jump to text segment
    jump_to_text_seg(file);

    // alloc space for 500 symbole
    nof_symbol_resize_segment(symbol_seg_ptr, 100);
    nof_text_resize_segment(text_seg_ptr, 100);
    
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
                unsigned int index = header->text_segment_size / BC_OPCODE_SIZE;
                //nof_text_set_instruction(*text_seg_ptr, index, BC_NOP);
                header->text_segment_size += BC_OPCODE_SIZE;
            }

            default: {
                // on function
                if (isletter(line[0])) {
                    add_symbol(header, symbol_seg_ptr, line, text_ptr);
               }
            }
        }
    }
}
