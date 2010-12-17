#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "container.h"
#include "bytecode.h"
#include "assembly.h"



void print_usage();

int main(int argc, char* argv[]) {
    // usage
    if (argc < 3) {
        print_usage();
        return 0;
    }

    // input
    char* input_path = argv[1];
    FILE* input = fopen(input_path, "r");
    if (input == NULL) {
        fprintf(stderr, "can not open file: %s\n", input_path);
        return 1;
    }

    // ouput
    char* output_path = argv[2];
    FILE* output = fopen(output_path, "w");
    if (output == NULL) {
        fprintf(stderr, "can not open file: %s\n", output_path);
        return 1;
    }

    // collection info from input
    asm_info_t info = asm_collection_info(input);

    // show info
    printf("data_section        = %u\n", info.data_section);
    printf("text_section        = %u\n", info.text_section);
    printf("symbol_count        = %u\n", info.symbol_count);
    printf("symbol_segment_size = %u\n", info.symbol_segment_size);
    printf("instruction_count   = %u\n", info.instruction_count);
    printf("text_segment_size   = %u\n", info.text_segment_size);

    // set header
    nof_header_t header;
    header.magic_number = NOF_MAGIC_NUMBER;
    header.container_version = NOF_CONTAINER_VERSION;
    header.content_version = BC_BYTECODE_VERSION;
    header.symbol_segment_size = info.symbol_segment_size;
    header.data_segment_size = 4;
    header.text_segment_size = info.text_segment_size;
    char buffer[4];

    // assembler
    char symbol_segment[header.symbol_segment_size];
    char text_segment[header.text_segment_size];
    asm_fill_segment(input, symbol_segment, text_segment, info);

    // write container
    nof_write_segment(output, header, symbol_segment, buffer, text_segment);

    free(symbol_segment);
    fclose(input);
    fclose(output);

    return 0;
}

void print_usage() {
    printf("usage\n");
}
