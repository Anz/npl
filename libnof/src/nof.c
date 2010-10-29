#include "nof.h"
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

// parse symbol
int nof_read_symbol(char* segment, size_t size, nof_symbol_t* symbol) {
    static char* start;
    static char* end;

    if (end != segment + size) {
        start = segment;
        end = segment + size;
    }

    if (end - start > 4) {
        symbol->pointer = swap_endian(*(unsigned int*)start);
        symbol->name = start + 4;
        start += 4 + strlen(symbol->name) + 1;
        return 1;
    } else {
        start = NULL;
        end = NULL;
        return 0;
    }
}

// write symbol segment
void nof_write_symbol(char* segment, nof_symbol_t symbol) {
    static char* start;
    static char* pointer;

    if (start != segment) {
        start = segment;
        pointer = segment;
    }

    symbol.pointer = swap_endian(symbol.pointer);
    memcpy(pointer, &symbol.pointer, 4);
    strcpy(pointer+4, symbol.name);
    pointer += 4 + strlen(symbol.name) + 1;
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

