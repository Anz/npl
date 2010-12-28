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

    // assembler
    ctr_header_t header;
    ctr_segment_t symbol_segment;
    ctr_segment_t data_segment;
    ctr_segment_t text_segment;
    assembler(input, &header, &symbol_segment, &data_segment, &text_segment);

    // write container
    ctr_write_segment(output, symbol_segment, data_segment, text_segment);
    free(symbol_segment.data);
    free(data_segment.data);
    free(text_segment.data);

    fclose(input);
    fclose(output);

    return 0;
}

void print_usage() {
    printf("usage\n");
}
