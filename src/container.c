#include "bytecode.h"
#include "container.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// read header
ctr_header_t ctr_read_header(FILE* file) {
    ctr_header_t header;

    // segment
    unsigned int segment[CTR_HEADER_SIZE / sizeof(int)];
    fseek(file, 0, SEEK_SET);
    fread(segment, sizeof(char), CTR_HEADER_SIZE, file);

    // init header
    header.magic_number = swap_endian(segment[0]);
    header.container_version = swap_endian(segment[1]); 
    header.content_version = swap_endian(segment[2]);
    header.symbol_segment_size = swap_endian(segment[3]);
    header.external_symbol_segment_size = swap_endian(segment[4]);
    header.text_segment_size = swap_endian(segment[5]);

   return header;
}

// read symbol segment into array
ctr_symbol_t* ctr_symbol_read(FILE* stream, ctr_header_t header) {
    int count = header.symbol_segment_size / CTR_SYMBOL_SIZE;
    ctr_symbol_t* symbols = calloc(count, sizeof(ctr_symbol_t));
    fseek(stream, CTR_HEADER_SIZE, SEEK_SET);
    for (int i = 0; i < count; i++) {
        char buffer[CTR_SYMBOL_SIZE];
        fread(buffer, sizeof(char), CTR_SYMBOL_SIZE, stream);
        symbols[i].addr = swap_endian(*(unsigned int*)buffer);
        memcpy(symbols[i].name, &buffer[4], CTR_SYMBOL_NAME_SIZE);
    }
    return symbols;
}

// find symbol in array by addr
int ctr_symbol_find(ctr_symbol_t* symbols, ctr_header_t header, ctr_addr addr) {
    int count = header.symbol_segment_size / CTR_SYMBOL_SIZE;
    for (int i = 0; i < count; i++) {
        if (symbols[i].addr == addr) {
            return i;
        }
    }
    return -1;
}

ctr_bytecode_t ctr_text_read(FILE* file, ctr_header_t header, unsigned int index) {
    unsigned int offset = ctr_text_offset(header) + index * BC_OPCODE_SIZE;

    // set cursor
    fseek(file, offset, SEEK_SET);

    // read
    char buffer[BC_OPCODE_SIZE];
    fread(buffer, BC_OPCODE_SIZE, 1, file);

    // fill data
    ctr_bytecode_t bc;
    bc.instruction = buffer[0];
    bc.argument = swap_endian(*(int*)&buffer[1]);

    return bc;
}

unsigned int ctr_text_count(ctr_header_t header) {
    return header.text_segment_size / BC_OPCODE_SIZE;
}

unsigned int ctr_text_offset(ctr_header_t header) {
    return CTR_HEADER_SIZE + header.symbol_segment_size + header.external_symbol_segment_size;
}
