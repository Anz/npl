#include <stdio.h>
#include <stdlib.h>
#include <container.h>
#include <bytecode.h>
#include "util.h"

void print_usage();

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
    printf("magic number        = 0x%X\n", header.magic_number);
    printf("container version   = %u\n", header.container_version);
    printf("content version     = %u\n", header.content_version);
    printf("symbol segment size = %u Bytes\n", header.symbol_segment_size);
    printf("data segment size   = %u Bytes\n", header.data_segment_size);
    printf("text segment size   = %u Bytes\n", header.text_segment_size);

    // check magic number
    if (header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    printf("symbol segment:\n");
    ctr_segment_t symbol_segment = ctr_read_symbol_segment(file, header);
    unsigned int symbol_count = ctr_symbol_count(symbol_segment);
    for(unsigned int index = 0; index < symbol_count; index++) {
        char name[CTR_SYMBOL_NAME_SIZE+1];
        ctr_symbol_get_name(symbol_segment, index, name);
        ctr_addr addr = ctr_symbol_get_addr(symbol_segment, index);
        
        printf("0x%08X = \"%s\"\n", addr, name);
    }
    printf("\n");

    // print text
    printf("text segment:\n");
    unsigned int text_count = header.text_segment_size / BC_OPCODE_SIZE;
    char opcode[BC_OPCODE_SIZE];
    long offset = CTR_HEADER_SIZE + header.symbol_segment_size + header.data_segment_size;
    fseek(file, offset, SEEK_SET);
    for(unsigned int i = 0; i < text_count; i++) {
        fread(opcode, sizeof(char), BC_OPCODE_SIZE, file);

        int symbol = ctr_symbol_find_by_addr(symbol_segment, i);

        if (symbol != -1) {
            char name[CTR_SYMBOL_NAME_SIZE+1];
            ctr_symbol_get_name(symbol_segment, symbol, name);
            printf("%08X <%s>:\n", i, name);
        }

        printf(" %08X:\t%02X %02X %02X %02X %02X\t\t%s\t0x%08X\n", 
            i, 
            char2int(opcode[0]), 
            char2int(opcode[1]), 
            char2int(opcode[2]), 
            char2int(opcode[3]), 
            char2int(opcode[4]), 
            bc_op2asm(opcode[0]), 
            swap_endian(*(unsigned int*)&opcode[1]));
    }
    printf("\n");

    fclose(file);
    return 0;
}

void print_usage() {
    printf("usage\n");
}
