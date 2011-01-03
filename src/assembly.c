#include <string.h>
#include "assembly.h"
#include "bytecode.h"
#include "util.h"
#include "map.h"

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

size_t write_symbol_segment(FILE* input, FILE* output, map_t* symbols) {
    // jump to text seg
    jump_to_text_seg(input);

    size_t size = 0;
    ctr_addr addr = 0;
    char line[READ_BUFFER_SIZE];
    while(!feof(input)) {
        fgets(line, READ_BUFFER_SIZE, input);

        char* colon = strchr(line, ':');
        char* instruction = NULL;
        if (colon == NULL) {
            instruction = strtok(line, " \t\r\n");
        } else {
            instruction = strtok(colon+1, " \t\r\n"); 
        }

        if (colon != NULL) {
            ctr_addr symbol_addr = swap_endian(addr);
            char* name = strtok(line, ": ");

            // add symbol to map
            map_add(symbols, name, &symbol_addr);

            // write to file
            fwrite(&symbol_addr, sizeof(char), CTR_ADDR_SIZE, output);
            fwrite(name, sizeof(char), CTR_SYMBOL_NAME_SIZE, output);
            size += CTR_SYMBOL_SIZE;
           
        }

        if (instruction != NULL) {
            addr++;
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
    map_t symbols;
    map_init(&symbols, sizeof(ctr_addr));
    size_t symbol_size = write_symbol_segment(input, output, &symbols);

    // jump to text segment
    jump_to_text_seg(input);

    size_t text_size = 0;
    char line[READ_BUFFER_SIZE];
    int running = 1;
    while(!feof(input) && running) {
        fgets(line, READ_BUFFER_SIZE, input);

        if (line[0] == ASM_SEGMENT) {
            running = 0;
            continue;
        }

        char* colon = strchr(line, ':');
        char* mnemonic = NULL;
        if (colon == NULL) {
            mnemonic = strtok(line, " \t\r\n");
        } else {
            mnemonic = strtok(colon+1, " \t\r\n"); 
        }


        // on instruction
        if (mnemonic != NULL) {
            char instruction = bc_asm2op(mnemonic);
            unsigned int arg = 0x0;

            char* arg1 = strtok(NULL, " ,\t\r\n"); 
            if (instruction == BC_SYNC && arg1 != NULL) {
                map_node_t* symbol = map_find(&symbols, arg1);
                if (symbol != NULL) {
                    arg = text_size - (*(ctr_addr*)symbol->value);
                }
            }

            // write
            fwrite(&instruction, sizeof(char), 1, output);
            fwrite(&arg, sizeof(int), 1, output);
            text_size += BC_OPCODE_SIZE;
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
