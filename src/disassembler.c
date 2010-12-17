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

    // load nof
    nof_header_t header = nof_read_header(file);

    // print header
    printf("header segment:\n");
    printf("magic number        = 0x%X\n", header.magic_number);
    printf("container version   = %u\n", header.container_version);
    printf("content version     = %u\n", header.content_version);
    printf("symbol segment size = %u Bytes\n", header.symbol_segment_size);
    printf("data segment size   = %u Bytes\n", header.data_segment_size);
    printf("text segment size   = %u Bytes\n", header.text_segment_size);

    // check magic number
    if (header.magic_number != NOF_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", NOF_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    printf("symbol segment:\n");
    char* symbol_segment = nof_read_symbol_segment(file, header);
    nof_symbol_t symbol;
    while(nof_read_symbol(symbol_segment, header.symbol_segment_size, &symbol)) {
        printf("0x%08X = \"%s\"\n", symbol.pointer, symbol.name);
    }
    printf("\n");

    // print text
    printf("text segment:\n");
    char* text_segment = nof_read_text_segment(file, header);
    nof_read_symbol(symbol_segment, header.symbol_segment_size, &symbol);
    for(int i = 0; i < header.text_segment_size; i += 5) {
        if (symbol.pointer == i) {
            printf("%s:\n", symbol.name);
            nof_read_symbol(symbol_segment, header.symbol_segment_size, &symbol);
        }

        printf("\t%s\n", bc_op2asm(text_segment[i]));
    }
    printf("\n");

    fclose(file);
    return 0;
}

void print_usage() {
    printf("usage\n");
}
