#include <string.h>
#include "assembly.h"
#include "bytecode.h"
#include "util.h"

#define READ_BUFFER_SIZE 1024
#define SEGMENT_BUFFER_SIZE 1024

void jump_to_text_seg(FILE* file) {
    fseek(file, 0, SEEK_SET);
    char line[READ_BUFFER_SIZE];
    memset(line, 0, READ_BUFFER_SIZE);
    while(!feof(file) && memcmp(line, ".text", 5) != 0)  {
        fgets(line, READ_BUFFER_SIZE, file);
    }
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

size_t write_symbol_segment(FILE* input, FILE* output) {
    size_t size = 0;
    ctr_addr addr = 0;
    char line[READ_BUFFER_SIZE];
    while(!feof(input)) {
        fgets(line, READ_BUFFER_SIZE, input);

        switch(line[0]) {
            case ASM_SPACE:
            case ASM_TAB: {
                addr++;
            }
            default: {
                // on function
                if (isletter(line[0])) {
                    ctr_addr symbol_addr = swap_endian(addr);
                    fwrite(&symbol_addr, sizeof(char), CTR_ADDR_SIZE, output);
                    char* name = strtok(line, ": ");
                    fwrite(name, sizeof(char), CTR_SYMBOL_NAME_SIZE, output);
                    size += CTR_SYMBOL_SIZE;
               }
            }
        }
    }
    return size;
}

void assembler(FILE* input, FILE* output) {
    // write tmp header
    char tmp_header[CTR_HEADER_SIZE];
    memset(tmp_header, 0, CTR_HEADER_SIZE);
    fwrite(tmp_header, sizeof(char), CTR_HEADER_SIZE, output);

    // write symbol
    size_t symbol_size = write_symbol_segment(input, output);

    // jump to text segment
    jump_to_text_seg(input);

    size_t text_size = 0;
    char line[READ_BUFFER_SIZE];
    int running = 1;
    while(!feof(input) && running) {
        fgets(line, READ_BUFFER_SIZE, input);

        switch(line[0]) {
            // abort on new segment
            case ASM_SEGMENT: {
                running = 0;
            }
            // on instruction
            case ASM_SPACE:
            case ASM_TAB: {
                char* mnemonic = strtok(line, "\t ");
                char instruction = bc_asm2op(mnemonic);
                fwrite(&instruction, sizeof(char), 1, output);
                unsigned int null = 0x00000000;
                fwrite(&null, sizeof(unsigned int), 1, output);
                text_size += BC_OPCODE_SIZE;
            }
        }
    }
    unsigned int header[] = {
        swap_endian(CTR_MAGIC_NUMBER),
        swap_endian(CTR_CONTAINER_VERSION),
        swap_endian(BC_BYTECODE_VERSION),
        swap_endian(symbol_size),
        swap_endian(0),
        swap_endian(text_size)
    };
    fseek(output, 0, SEEK_SET);
    fwrite(header, sizeof(char), CTR_HEADER_SIZE, output);
}
