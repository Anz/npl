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

    // set header
    nof_header_t header;
    header.magic_number = NOF_MAGIC_NUMBER;
    header.container_version = NOF_CONTAINER_VERSION;
    header.content_version = BC_BYTECODE_VERSION;
    header.symbol_segment_size = 0;
    header.data_segment_size = 0;
    header.text_segment_size = 0;

    // assembler
    char* symbol_segment = NULL;
    char* data_segment = NULL;
    char* text_segment = NULL;
    assembler(input, &header, &symbol_segment, &data_segment, &text_segment);

    // write container
    nof_write_segment(output, header, symbol_segment, NULL, text_segment);
    free(symbol_segment);
    free(data_segment);
    free(text_segment);

    fclose(input);
    fclose(output);

    return 0;
}

void print_usage() {
    printf("usage\n");
}
