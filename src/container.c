#include "bytecode.h"
#include "container.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// read segment
char* read_segment(FILE* file, size_t start, size_t size) {
    char* segment = malloc(size);
    memset(segment, 0, size);

    // jump to symbol section
    fseek(file, start, SEEK_SET);

    // read segment
    fread(segment, sizeof(char), size, file);

    return segment;
}

// get info from headre
unsigned int get_header_info(char* header, unsigned int index) {
    int* pointer = (int*)&header[index];
    unsigned int value = swap_endian(*pointer);
    return value;
}

// read header
nof_header_t nof_read_header(FILE* file) {
    nof_header_t header;

    // segment
    char* segment = read_segment(file, 0, NOF_HEADER_SIZE);

    // init header
    header.magic_number        = get_header_info(segment, NOF_MAGIC_NUMBER_INDEX);
    header.container_version   = get_header_info(segment, NOF_CONTAINER_VERSION_INDEX);
    header.content_version     = get_header_info(segment, NOF_CONTENT_VERSION_INDEX);
    header.symbol_segment_size = get_header_info(segment, NOF_SYMBOL_SIZE_INDEX);
    header.data_segment_size   = get_header_info(segment, NOF_DATA_SIZE_INDEX);
    header.text_segment_size   = get_header_info(segment, NOF_TEXT_SIZE_INDEX);

    return header;
}


// read symbol section
char* nof_read_symbol_segment(FILE* file, nof_header_t header) {
    size_t start = NOF_HEADER_SIZE;
    size_t size = header.symbol_segment_size;
    return read_segment(file, start, size);
}

// read data section
char* nof_read_data_segment(FILE* file, nof_header_t header) {
    size_t start = NOF_HEADER_SIZE + header.symbol_segment_size;
    size_t size = header.data_segment_size;
    return read_segment(file, start, size);
}

// read text section
char* nof_read_text_segment(FILE* file, nof_header_t header) {
    size_t start = NOF_HEADER_SIZE + header.symbol_segment_size + header.data_segment_size;
    size_t size = header.text_segment_size;
    return read_segment(file, start, size);
}

// calculate symbol count
// symbol count = symbol size / (symbol addr size + symbol name size)
unsigned int nof_symbol_count(nof_header_t header) {
    return header.symbol_segment_size / NOF_SYMBOL_SIZE;
}

// get symbol address from symbol segment by index
ctr_addr nof_symbol_get_addr(char* segment, unsigned int index) {
    ctr_addr* ptr = (ctr_addr*)&segment[index * NOF_SYMBOL_SIZE];
    ctr_addr addr = *ptr;
    return swap_endian(addr);
}

// get symbol address from symbol segment by index
void nof_symbol_set_addr(char* segment, unsigned int index, ctr_addr addr) {
    addr = swap_endian(addr);
    ctr_addr* ptr = (ctr_addr*)&segment[index * NOF_SYMBOL_SIZE];
    *ptr = addr;
}

// get symbol name from symbol segment by index
void nof_symbol_get_name(char* segment, unsigned int index, char* name) {
    memset(name, 0, NOF_SYMBOL_NAME_SIZE + 1);
    char* ptr = &segment[index * NOF_SYMBOL_SIZE + NOF_ADDR_SIZE];
    memcpy(name, ptr, NOF_SYMBOL_NAME_SIZE);
}

// set symbol name from symbol segment by index
void nof_symbol_set_name(char* segment, unsigned int index, char* name) {
    char* symbol_name = &segment[index * NOF_SYMBOL_SIZE + NOF_ADDR_SIZE];
    memset(symbol_name, 0, NOF_SYMBOL_NAME_SIZE);
    strncpy(symbol_name, name, NOF_SYMBOL_NAME_SIZE);
}

// resize symbol segment by count of symbols
void nof_symbol_resize_segment(char** segment, unsigned int count) {
    char* old_segment = *segment;
    *segment = realloc(*segment, count * NOF_SYMBOL_SIZE);
    if (*segment == NULL) {
        free(old_segment);
    }
}

// count all elements
unsigned int nof_text_count(nof_header_t header) {
    return header.text_segment_size / BC_OPCODE_SIZE;
}

// resize text segment by count of instructon
void nof_text_resize_segment(char** segment, unsigned int count) {
    char* old_segment = *segment;
    *segment = realloc(*segment, count * BC_OPCODE_SIZE);
    if (*segment == NULL) {
        free(old_segment);
    }
}

// get instruction by index
char nof_text_get_instruction(char* segment, unsigned int index) {
    return segment[index * BC_OPCODE_SIZE];
}

// set instruction by index
void nof_text_set_instruction(char* segment, unsigned int index, char instruction) {
    segment[index * BC_OPCODE_SIZE] = instruction;
}

// write to file
void nof_write_segment(FILE* file, nof_header_t header, char* symbol, char* data, char* text) {
    // handle endian
    unsigned int magic_number      = swap_endian(header.magic_number);
    unsigned int container_version = swap_endian(header.container_version);
    unsigned int content_version   = swap_endian(header.content_version);
    size_t symbol_segment_size     = swap_endian(header.symbol_segment_size);
    size_t data_segment_size       = swap_endian(header.data_segment_size);
    size_t text_segment_size       = swap_endian(header.text_segment_size);

    // write header
    fwrite(&magic_number, 4, 1, file);
    fwrite(&container_version, 4, 1, file);
    fwrite(&content_version, 4, 1, file);
    fwrite(&symbol_segment_size, 4, 1, file);
    fwrite(&data_segment_size, 4, 1, file);
    fwrite(&text_segment_size, 4, 1, file);

    // write symbol
    fwrite(symbol, 1, header.symbol_segment_size, file);

    // write data
    fwrite(data, 1, header.data_segment_size, file);

    // write text
    fwrite(text, 1, header.text_segment_size, file);
}

