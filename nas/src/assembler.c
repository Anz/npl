#include "assembler.h"
#include <string.h>
#include <bytecode.h>

#define READ_BUFFER_SIZE 500

asm_info_t asm_collection_info(FILE* file) {
    asm_info_t info;
    info.data_section = 0;
    info.text_section = 0;
    info.symbol_count = 0;
    info.symbol_segment_size = 0;
    info.instruction_count = 0;
    info.text_segment_size = 0;

    // jump to start
    fseek(file, 0, SEEK_SET);

    char line[READ_BUFFER_SIZE];
    while(!feof(file)) {
        fgets(line, READ_BUFFER_SIZE, file);

        switch(line[0]) {
            // ingore comment
            case ASM_COMMENT: continue;
            // on segment
            case ASM_SEGMENT: {
                if (memcmp(ASM_DATA_SEGMENT, &line[1], strlen(ASM_DATA_SEGMENT)) == 0) {
                    info.data_section = ftell(file);
                } else if (memcmp(ASM_TEXT_SEGMENT, &line[1], strlen(ASM_TEXT_SEGMENT)) == 0) {
                    info.text_section = ftell(file);
                }
                break;
            }
            // on instruction
            case ASM_INSTRUCTION: {
                info.instruction_count++;
                break;
            }
            // on function
            default: {
                if (96 < line[0] && line[0] < 123) { // function
                    info.symbol_count++;
                    info.symbol_segment_size += 4 + (strchr(line, ':') - line) + 1;
                }
                break;
            }
        }
    }

    info.text_segment_size = info.instruction_count * 5;
    return info;
}

void asm_fill_segment(FILE* file, char* symbol_segment, char* text_segment, asm_info_t info) {
    // jump to .text
    fseek(file, info.text_section, SEEK_SET);

    unsigned int instruction_index = 0;
    //char* symbol_pointer = symbol;

    char line[READ_BUFFER_SIZE];
    while(!feof(file)) {
        fgets(line, READ_BUFFER_SIZE, file);

        switch (line[0]) {
            // ignore comment
            case ASM_COMMENT: continue;
            // abort on new segment
            case ASM_SEGMENT: return;
            // on instruction
            case ASM_INSTRUCTION: {
                char* mnemonic = strtok(line, "\t ");
                char instruction = bc_asm2op(mnemonic);
                text_segment[instruction_index] = instruction;
                unsigned int addr = 0xAAAAAAAA;
                memcpy(&text_segment[instruction_index+1], &addr, 4);
                instruction_index += 5;
                break;
            }
            // on function
            default: {
                if (96 < line[0] && line[0] < 123) { // function
                    nof_symbol_t symbol;
                    symbol.pointer = instruction_index;
                    size_t len = (strchr(line, ':') - line) + 1;
                    char name[len];
                    memcpy(name, line, len);
                    name[len-1] = '\0';
                    symbol.name = name;
                    nof_write_symbol(symbol_segment, symbol);
                }
            }
        }
    }
}
