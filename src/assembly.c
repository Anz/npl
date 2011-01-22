#include <string.h>
#include <stdlib.h>
#include "assembly.h"
#include "bytecode.h"
#include "util.h"
#include "map.h"
#include "list.h"

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

size_t write_symbol_segment(FILE* input, FILE* output, map_t* symbols, map_t* externals) {
    // jump to text seg
    jump_to_text_seg(input);

    size_t size = 0;
    ctr_addr addr = 0;
    char line[READ_BUFFER_SIZE];
    int external_index = 0;

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
            char* token = strtok(line, ": ");

            // add symbol to map
            char name[CTR_SYMBOL_NAME_SIZE];
            memset(name, 0, CTR_SYMBOL_SIZE);
            strcpy(name, token);
            map_set(symbols, &addr, name);

            // write to file
            fwrite(&symbol_addr, sizeof(char), CTR_ADDR_SIZE, output);
            fwrite(name, sizeof(char), CTR_SYMBOL_NAME_SIZE, output);
            size += CTR_SYMBOL_SIZE;
           
        }

        if (instruction != NULL) {
            char code = bc_asm2op(instruction);
            if (code == BC_SYNCE || code == BC_ASYNCE) {
                char* arg = strtok(NULL, " ,\t\r\n");
                char name[CTR_SYMBOL_NAME_SIZE];
                memset(name, 0, CTR_SYMBOL_NAME_SIZE);
                strcpy(name, arg);
                map_set(externals, &external_index, name);
            }
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
    map_init(&symbols, sizeof(ctr_addr), CTR_SYMBOL_NAME_SIZE);
    map_t externals;
    map_init(&externals, sizeof(int), CTR_SYMBOL_NAME_SIZE);
    size_t symbol_size = write_symbol_segment(input, output, &symbols, &externals);

    // write external symbol
    size_t external_size = externals.list.count * CTR_SYMBOL_NAME_SIZE;
    for (unsigned int i = 0; i < externals.list.count; i++) {
        map_entry_t* entry = list_get(&externals.list, i);
        fwrite(entry->value, sizeof(char), CTR_SYMBOL_NAME_SIZE, output);
    }

    // jump to text segment
    jump_to_text_seg(input);

    // variables
    map_t variables;
    map_init(&variables, sizeof(int), CTR_SYMBOL_NAME_SIZE);

    size_t text_size = 0;
    char line[READ_BUFFER_SIZE];
    int running = 1;
    while(!feof(input) && running) {
        memset(line, 0, READ_BUFFER_SIZE);
        fgets(line, READ_BUFFER_SIZE, input);

        if (line[0] == ASM_SEGMENT) {
            running = 0;
            continue;
        }

        if (line[0] == ASM_COMMENT) {
            continue;
        }

        char* colon = strchr(line, ':');
        char* mnemonic = NULL;
        if (colon == NULL) {
            mnemonic = strtok(line, ", \t\r\n");
        } else {
            mnemonic = strtok(colon+1, ", \t\r\n"); 
        }


        // on instruction
        if (mnemonic != NULL) {
            char instruction = bc_asm2op(mnemonic);
            unsigned int arg = 0x0;

            char* arg1 = strtok(NULL, ", \t\r\n"); 
            if (arg1 != NULL) {
                switch (instruction) {
                    case BC_SYNC: {
                        ctr_addr* addr = (int*)map_find_value(&symbols, arg1);
                        if (addr != NULL) {
                            // maybe **addr
                            arg = *addr - text_size / BC_OPCODE_SIZE;
                        } else {
                            arg = 0xFFFFFFFF;
                        }
                        break;
                    }
                    case BC_SYNCE:
                    case BC_ASYNCE: { 
                        char name[CTR_SYMBOL_NAME_SIZE];
                        memset(name, 0, CTR_SYMBOL_SIZE);
                        strcpy(name, arg1);
                        int* index = map_find_value(&externals, name);
                        if (index) {
                             arg = *index;
                        } else {
                            arg = 0xFFFFFFFF;
                        }
                        break;
                    }
                    case BC_ENTER: {
                        int variable_index = 0;
                        while(arg1 != NULL) {
                            arg += 4;
                            char name[CTR_SYMBOL_NAME_SIZE];
                            memset(name, 0, CTR_SYMBOL_SIZE);
                            strcpy(name, arg1);
                            map_set(&variables, &variable_index, name);
                            arg1 = strtok(NULL, ", \t\r\n");
                            variable_index++;
                        }
                        break;
                    }
                    case BC_ARG: {
                        char name[CTR_SYMBOL_NAME_SIZE];
                        memset(name, 0, CTR_SYMBOL_SIZE);
                        strcpy(name, arg1);
                        ctr_addr* variable = map_find_value(&variables, name);
                        if (variable) {
                            arg = *variable;
                        } else {
                            fprintf(stderr, "error could not find variable: %s\n", arg1);
                        }
                        break;
                    }
                    case BC_ARGV: {
                        arg = atoi(arg1);
                        break;
                    }
                }
            }

            // write
            fwrite(&instruction, sizeof(char), 1, output);
            arg = swap_endian(arg);
            fwrite(&arg, sizeof(int), 1, output);
            text_size += BC_OPCODE_SIZE;
        }
    }

    // release
    map_release(&symbols);
    map_release(&externals);
    map_release(&variables);

    unsigned int header[] = {
        swap_endian(CTR_MAGIC_NUMBER),
        swap_endian(CTR_CONTAINER_VERSION),
        swap_endian(BC_BYTECODE_VERSION),
        swap_endian(symbol_size),
        swap_endian(external_size),
        swap_endian(text_size)
    };
    fseek(output, 0, SEEK_SET);
    fwrite(header, sizeof(char), CTR_HEADER_SIZE, output);
}
