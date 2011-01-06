#include <stdio.h>
#include <stdlib.h>
#include <container.h>
#include <bytecode.h>
#include "util.h"
#include "map.h"
#include "list.h"

void print_usage();
list_t collect_external_symbols(FILE* file, ctr_header_t header);
void print_external_symbol_segment(list_t external_symbol);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }

    // load file
    char* path = argv[1];
    FILE* file = fopen(path, "rb");

    // on error, print error
    if (file == NULL) {
        printf("could not open file: %s\n", path);
        return 1;
    }

    // load ctr
    ctr_header_t header = ctr_read_header(file);

    // print header
    printf("header segment:\n");
    printf("magic number                   = 0x%X\n", header.magic_number);
    printf("container version              = %u\n", header.container_version);
    printf("content version                = %u\n", header.content_version);
    printf("symbol segment size            = %u Bytes\n", header.symbol_segment_size);
    printf("external symbol segment size   = %u Bytes\n", header.external_symbol_segment_size);
    printf("text segment size              = %u Bytes\n", header.text_segment_size);

    // check magic number
    if (header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    map_t symbols;
    map_init(&symbols, CTR_SYMBOL_NAME_SIZE+1);
    printf("symbol segment:\n");
    ctr_segment_t symbol_segment = ctr_read_symbol_segment(file, header);
    unsigned int symbol_count = ctr_symbol_count(symbol_segment);
    for(unsigned int index = 0; index < symbol_count; index++) {
        char name[CTR_SYMBOL_NAME_SIZE+1];
        ctr_symbol_get_name(symbol_segment, index, name);
        ctr_addr addr = ctr_symbol_get_addr(symbol_segment, index);
        char addr_key[9];
        sprintf(addr_key, "%x", addr);
        map_add(&symbols, addr_key, name); 
        printf("0x%08X = \"%s\"\n", addr, name);
    }
    printf("\n");

    // print external symbols
    list_t external_symbols = collect_external_symbols(file, header);
    print_external_symbol_segment(external_symbols);

    // print text
    printf("text segment:\n");
    unsigned int text_count = header.text_segment_size / BC_OPCODE_SIZE;
    char opcode[BC_OPCODE_SIZE];
    long offset = CTR_HEADER_SIZE + header.symbol_segment_size + header.external_symbol_segment_size;
    fseek(file, offset, SEEK_SET);
    for(unsigned int i = 0; i < text_count; i++) {
        fread(opcode, sizeof(char), BC_OPCODE_SIZE, file);

        int symbol = ctr_symbol_find_by_addr(symbol_segment, i);

        if (symbol != -1) {
            char name[CTR_SYMBOL_NAME_SIZE+1];
            ctr_symbol_get_name(symbol_segment, symbol, name);
            printf("%08X <%s>:\n", i, name);
        }

        char instruction = opcode[0];
        int arg = swap_endian(*(int*)&opcode[1]);

        printf(" %08X:\t%02X %02X %02X %02X %02X\t\t%s\t",
                i, 
                char2int(opcode[0]), 
                char2int(opcode[1]), 
                char2int(opcode[2]), 
                char2int(opcode[3]), 
                char2int(opcode[4]),
                bc_op2asm(opcode[0]));

        switch (instruction) {
            case BC_SYNC:
            case BC_ASYNC: {
                char addr_key[9]; 
                sprintf(addr_key, "%x", i + arg);
                map_node_t* symbol_node = map_find(&symbols, addr_key);
                if (symbol_node != NULL) {
                    printf("%s\n", (char*)symbol_node->value);
                } else {
                    printf("0x%08X\n", arg);
                }
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                list_node symbol = list_get(&external_symbols, arg);
                if (symbol != NULL) {
                    printf("%s\n", (char*)list_data(symbol));
                } else {
                    printf("0x%08X\n", arg);
                }
                break;
            }
            default: {
                printf("0x%08X\n", arg);
                break;
            }
        }
    }
    printf("\n");

    // release
    list_release(&external_symbols);

    fclose(file);
    return 0;
}

void print_usage() {
    printf("usage: nis [options] <input>\n");
}

list_t collect_external_symbols(FILE* file, ctr_header_t header) {
    list_t symbols;
    list_init(&symbols, CTR_SYMBOL_SIZE+1);

    long offset = CTR_HEADER_SIZE + header.symbol_segment_size;
    size_t size = header.external_symbol_segment_size;

    // set cursor to segement start
    fseek(file, offset, SEEK_SET);

    // read line by line
    char symbol[CTR_SYMBOL_NAME_SIZE+1];
    symbol[CTR_SYMBOL_NAME_SIZE] = '\0';
    for (size_t i = 0; i < size; i += CTR_SYMBOL_NAME_SIZE) {
        fread(symbol, sizeof(char), CTR_SYMBOL_NAME_SIZE, file);
        list_add(&symbols, symbol);
    };
    return symbols;
}

void print_external_symbol_segment(list_t external_symbol) {
    printf("external symbol segment:\n\n");

    list_node symbol = list_first(&external_symbol);
    while (symbol != NULL) {
        char* name = list_data(symbol);
        printf("<%s>\n", name);
        symbol = list_next(symbol);
    }
    printf("\n");
}
