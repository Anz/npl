#include <stdio.h>
#include <stdlib.h>
#include <container.h>
#include <bytecode.h>

void print_usage();

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }

    // load file
    char* path = argv[1];
    FILE* file = fopen(path, "r");

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
    ctr_segment_t text_segment = ctr_read_text_segment(file, header);
    unsigned int text_count = ctr_text_count(text_segment);
    for(unsigned int i = 0; i < text_count; i++) {
        ctr_addr addr = i * BC_OPCODE_SIZE;
        int symbol = ctr_symbol_find_by_addr(symbol_segment, addr);
        char instruction = ctr_text_get_instruction(text_segment, i);

        if (symbol != -1) {
            char name[CTR_SYMBOL_NAME_SIZE+1];
            ctr_symbol_get_name(symbol_segment, symbol, name);
            printf("%s:\t%s\n", name, bc_op2asm(instruction));
        } else {
            printf("\t%s\n", bc_op2asm(instruction));
        }
    }
    printf("\n");

    fclose(file);
    return 0;
}

void print_usage() {
    printf("usage\n");
}
