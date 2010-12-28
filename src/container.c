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

// read header
ctr_header_t ctr_read_header(FILE* file) {
    ctr_header_t header;

    // segment
    ctr_segment_t segment = read_segment(file, 0, CTR_HEADER_SIZE);
    unsigned int* ptr = (unsigned int*)segment.data;

    // init header
    header.magic_number        = swap_endian(ptr[0]);
    header.container_version   = swap_endian(ptr[1]); 
    header.content_version     = swap_endian(ptr[2]);
    header.symbol_segment_size = swap_endian(ptr[3]);
    header.data_segment_size   = swap_endian(ptr[4]);
    header.text_segment_size   = swap_endian(ptr[5]);

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
    segment->size = count * CTR_SYMBOL_SIZE;
    segment->data = resize_memory(segment->data, segment->size);
    if (segment->data == NULL) {
        segment->size = 0;
    }
}

// find symbol by addr
int ctr_symbol_find_by_addr(ctr_segment_t segment, ctr_addr addr) {
    unsigned int count = ctr_symbol_count(segment);
    for (int i = 0; i < count; i++) {
        ctr_addr to_compare = ctr_symbol_get_addr(segment, i);
        if (to_compare == addr) {
            return i;
        }
    }
    return -1;
}

// count all elements
unsigned int ctr_text_count(ctr_segment_t segment) {
    return segment.size / BC_OPCODE_SIZE;
}

// resize text segment by count of instructon
void ctr_text_resize_segment(ctr_segment_t* segment, unsigned int count) {
    segment->size = count * BC_OPCODE_SIZE;
    segment->data = resize_memory(segment->data, segment->size);
    if (segment->data == NULL) {
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
    unsigned int header[] = { 
        swap_endian(CTR_MAGIC_NUMBER),
        swap_endian(CTR_CONTAINER_VERSION),
        swap_endian(BC_BYTECODE_VERSION),
        swap_endian(symbol.size),
        swap_endian(data.size),
        swap_endian(text.size) 
    };

    // write header
    fwrite(header, 1, sizeof(header), file);

    // write symbol
    fwrite(symbol.data, 1, symbol.size, file);

    // write data
    fwrite(data.data, 1, data.size, file);

    // write text
    fwrite(text.data, 1, text.size, file);
}

