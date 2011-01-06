#include <stdio.h>
#include <stdlib.h>
#include <container.h>
#include <bytecode.h>
#include "util.h"
#include "map.h"
#include "list.h"

void print_usage();
list_t collect_external_symbols(FILE* file, ctr_header_t header);
void print_header(ctr_header_t header);
void print_external_symbol_segment(list_t external_symbol, ctr_header_t header);

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
    print_header(header);

    // check magic number
    if (header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    map_t symbols;
    map_init(&symbols, CTR_SYMBOL_NAME_SIZE+1);
    printf("symbol segment:\n\n");
    printf("%10s\taddr     \tname\n\n", "");
    ctr_symbol_t* symbol_list = ctr_symbol_read(file, header);
    unsigned int symbol_count = header.symbol_segment_size / CTR_SYMBOL_SIZE;
    for(unsigned int index = 0; index < symbol_count; index++) {
        ctr_symbol_t* symbol = &symbol_list[index];
        map_addi(&symbols, symbol->addr, symbol->name);
        printf("%08X:\t%08X \t%s\n", CTR_HEADER_SIZE + index * CTR_SYMBOL_SIZE, symbol->addr, symbol->name);
    }
    printf("\n");

    // print external symbols
    list_t external_symbols = collect_external_symbols(file, header);
    print_external_symbol_segment(external_symbols, header);

    // print text
    printf("text segment:\n\n");
    printf("%11s\taddr      \tbinary        \tname\targument\n\n", "");
    unsigned int text_count = ctr_text_count(header);
    int offset = ctr_text_offset(header);
    for(unsigned int i = 0; i < text_count; i++) {
        ctr_bytecode_t bc = ctr_text_read(file, header, i);

        map_node_t* symbol = map_findi(&symbols, i);
        if (symbol != NULL) {
            printf("%08X <%s>:\n", i, (char*)symbol->value);
        }

        printf(" %08X:\t", offset + i * BC_OPCODE_SIZE);
        printf("%08X\t", i);
        printf("%02X ", char2int(bc.instruction));
        printf("%02X ", char2int(int2char(bc.argument, 0)));
        printf("%02X ", char2int(int2char(bc.argument, 1)));
        printf("%02X ", char2int(int2char(bc.argument, 2)));
        printf("%02X\t", char2int(int2char(bc.argument, 3)));
        printf("%s\t", bc_op2asm(bc.instruction));

        switch (bc.instruction) {
            case BC_SYNC:
            case BC_ASYNC: {
                map_node_t* symbol_node = map_findi(&symbols, i + bc.argument);
                if (symbol_node != NULL) {
                    printf("%s\n", (char*)symbol_node->value);
                } else {
                    printf("0x%08X\n", bc.argument);
                }
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                list_node symbol = list_get(&external_symbols, bc.argument);
                if (symbol != NULL) {
                    printf("%s\n", (char*)list_data(symbol));
                } else {
                    printf("0x%08X\n", bc.argument);
                }
                break;
            }
            default: {
                printf("\n");
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

void print_header(ctr_header_t header) {
    printf("header segment:\n\n");
    printf("%08X:\tmagic number 0x%X\n", 0, header.magic_number);
    printf("%08X:\tcontainer v%u\n", 4,  header.container_version);
    printf("%08X:\tcontent   v%u\n", 8, header.content_version);
    printf("%08X:\tsymbol    %4.2f KB (%u Bytes)\n", 12, header.symbol_segment_size / 1024.0, header.symbol_segment_size);
    printf("%08X:\texternal  %4.2f KB (%u Bytes)\n", 16, header.external_symbol_segment_size / 1024.0, header.external_symbol_segment_size);
    printf("%08X:\ttext      %4.2f KB (%u Bytes)\n", 20, header.text_segment_size / 1024.0, header.text_segment_size);
}

void print_external_symbol_segment(list_t external_symbol, ctr_header_t header) {
    size_t offset = CTR_HEADER_SIZE + header.symbol_segment_size;

    printf("external symbol segment:\n\n");
    printf("%10s\tname\n\n", "");

    list_node symbol = list_first(&external_symbol);
    int i = 0;
    while (symbol != NULL) {
        char* name = list_data(symbol);
        printf("%08X:\t%s\n", offset + i, name);
        symbol = list_next(symbol);
        i += CTR_SYMBOL_NAME_SIZE;
    }
    printf("\n");
}
