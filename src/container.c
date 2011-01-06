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
// symbol segment functions
ctr_symbol_t ctr_symbol_read(FILE* file, unsigned int index) {
    unsigned int offset = ctr_symbol_offset() + index * CTR_SYMBOL_SIZE;

    // set cursor
    fseek(file, offset, SEEK_SET);

    // read
    char buffer[CTR_SYMBOL_SIZE];
    fread(buffer, CTR_SYMBOL_SIZE, 1, file);

    // fill data
    ctr_symbol_t symbol;
    symbol.addr = swap_endian(*(int*)buffer);
    memcpy(symbol.name, &buffer[4], CTR_SYMBOL_NAME_SIZE);
    symbol.name[CTR_SYMBOL_NAME_SIZE] = '\0';

    return symbol;
}

unsigned int ctr_symbol_count(ctr_header_t header) {
    return header.symbol_segment_size / CTR_SYMBOL_SIZE;
}

unsigned int ctr_symbol_offset() {
    return CTR_HEADER_SIZE;
}

// external symbol segment functions
ctr_external_symbol_t ctr_external_read(FILE* file, ctr_header_t header, unsigned int index) {
    unsigned int offset = ctr_external_offset(header) + index * CTR_SYMBOL_NAME_SIZE;

    // set cursor
    fseek(file, offset, SEEK_SET);

    // read
    ctr_external_symbol_t symbol;
    fread(symbol.name, CTR_SYMBOL_SIZE, 1, file);
    symbol.name[CTR_SYMBOL_NAME_SIZE] = '\0';
    return symbol;
}

unsigned int ctr_external_count(ctr_header_t header) {
    return header.external_symbol_segment_size / CTR_SYMBOL_NAME_SIZE;
}

unsigned int ctr_external_offset(ctr_header_t header) {
    return CTR_HEADER_SIZE + header.symbol_segment_size;
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
