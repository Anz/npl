#include "bytecode.h"
#include "container.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// read segment
ctr_segment_t read_segment(FILE* file, size_t start, size_t size) {
    ctr_segment_t segment;
    segment.size = size;
    segment.data = malloc(size);
    memset(segment.data, 0, size);

    // jump to symbol section
    fseek(file, start, SEEK_SET);

    // read segment
    fread(segment.data, sizeof(char), size, file);

    return segment;
}

// get info from headre
unsigned int get_header_info(ctr_segment_t segment, unsigned int index) {
    unsigned int value = *(unsigned int*)&segment.data[index];
    return swap_endian(value);
}

// read header
ctr_header_t ctr_read_header(FILE* file) {
    ctr_header_t header;

    // segment
    ctr_segment_t segment = read_segment(file, 0, CTR_HEADER_SIZE);

    // init header
    header.magic_number        = get_header_info(segment, CTR_MAGIC_NUMBER_INDEX);
    header.container_version   = get_header_info(segment, CTR_CONTAINER_VERSION_INDEX);
    header.content_version     = get_header_info(segment, CTR_CONTENT_VERSION_INDEX);
    header.symbol_segment_size = get_header_info(segment, CTR_SYMBOL_SIZE_INDEX);
    header.data_segment_size   = get_header_info(segment, CTR_DATA_SIZE_INDEX);
    header.text_segment_size   = get_header_info(segment, CTR_TEXT_SIZE_INDEX);

    return header;
}


// read symbol section
ctr_segment_t ctr_read_symbol_segment(FILE* file, ctr_header_t header) {
    size_t start = CTR_HEADER_SIZE;
    size_t size = header.symbol_segment_size;
    return read_segment(file, start, size);
}

// read data section
ctr_segment_t ctr_read_data_segment(FILE* file, ctr_header_t header) {
    size_t start = CTR_HEADER_SIZE + header.symbol_segment_size;
    size_t size = header.data_segment_size;
    return read_segment(file, start, size);
}

// read text section
ctr_segment_t ctr_read_text_segment(FILE* file, ctr_header_t header) {
    size_t start = CTR_HEADER_SIZE + header.symbol_segment_size + header.data_segment_size;
    size_t size = header.text_segment_size;
    return read_segment(file, start, size);
}

// calculate symbol count
// symbol count = symbol size / (symbol addr size + symbol name size)
unsigned int ctr_symbol_count(ctr_segment_t segment) {
    return segment.size / CTR_SYMBOL_SIZE;
}

// get symbol index
ctr_addr* get_symbol_addr_ptr(ctr_segment_t segment, unsigned int index) {
    return (ctr_addr*)&segment.data[index * CTR_SYMBOL_SIZE];
}

// get symbol address from symbol segment by index
ctr_addr ctr_symbol_get_addr(ctr_segment_t segment, unsigned int index) {
    return swap_endian(*get_symbol_addr_ptr(segment, index));
}

// get symbol address from symbol segment by index
void ctr_symbol_set_addr(ctr_segment_t segment, unsigned int index, ctr_addr addr) {
    *get_symbol_addr_ptr(segment, index) = swap_endian(addr);
}

// get symbol name index
char* get_symbol_name_ptr(ctr_segment_t segment, unsigned int index) {
    return &segment.data[index * CTR_SYMBOL_SIZE + CTR_ADDR_SIZE];
}

// get symbol name from symbol segment by index
void ctr_symbol_get_name(ctr_segment_t segment, unsigned int index, char* name) {
    memset(name, 0, CTR_SYMBOL_NAME_SIZE + 1);
    memcpy(name, get_symbol_name_ptr(segment, index), CTR_SYMBOL_NAME_SIZE);
}

// set symbol name from symbol segment by index
void ctr_symbol_set_name(ctr_segment_t segment, unsigned int index, char* name) {
    char* symbol_name = get_symbol_name_ptr(segment, index);
    memset(symbol_name, 0, CTR_SYMBOL_NAME_SIZE);
    strncpy(symbol_name, name, CTR_SYMBOL_NAME_SIZE);
}

// resize symbol segment by count of symbols
void ctr_symbol_resize_segment(ctr_segment_t* segment, unsigned int count) {
    char* old_segment = segment->data;
    segment->size = count * CTR_SYMBOL_SIZE;
    segment->data = realloc(segment->data, segment->size);
    if (segment->data == NULL) {
        free(old_segment);
        segment->data = NULL;
        segment->size = 0;
    }
}

// count all elements
unsigned int ctr_text_count(ctr_segment_t segment) {
    return segment.size / BC_OPCODE_SIZE;
}

// resize text segment by count of instructon
void ctr_text_resize_segment(ctr_segment_t* segment, unsigned int count) {
    char* old_segment = segment->data;
    segment->size = count * BC_OPCODE_SIZE;
    segment->data = realloc(segment->data, segment->size);
    if (segment->data == NULL) {
        free(old_segment);
        segment->data = NULL;
        segment->size = 0;
    }
}

// get instruction by index
char ctr_text_get_instruction(ctr_segment_t segment, unsigned int index) {
    return segment.data[index * BC_OPCODE_SIZE];
}

// set instruction by index
void ctr_text_set_instruction(ctr_segment_t segment, unsigned int index, char instruction) {
    segment.data[index * BC_OPCODE_SIZE] = instruction;
}

// write to file
void ctr_write_segment(FILE* file, ctr_segment_t symbol, ctr_segment_t data, ctr_segment_t text) {
    // handle endian
    unsigned int magic_number      = swap_endian(CTR_MAGIC_NUMBER);
    unsigned int container_version = swap_endian(CTR_CONTAINER_VERSION);
    unsigned int content_version   = swap_endian(BC_BYTECODE_VERSION);
    size_t symbol_segment_size     = swap_endian(symbol.size);
    size_t data_segment_size       = swap_endian(data.size);
    size_t text_segment_size       = swap_endian(text.size);

    // write header
    fwrite(&magic_number, 4, 1, file);
    fwrite(&container_version, 4, 1, file);
    fwrite(&content_version, 4, 1, file);
    fwrite(&symbol_segment_size, 4, 1, file);
    fwrite(&data_segment_size, 4, 1, file);
    fwrite(&text_segment_size, 4, 1, file);

    // write symbol
    fwrite(symbol.data, 1, symbol.size, file);

    // write data
    fwrite(data.data, 1, data.size, file);

    // write text
    fwrite(text.data, 1, text.size, file);
}

