#ifndef CTR_H
#define CTR_H

#include <stddef.h>
#include <stdio.h>

// constans
#define CTR_MAGIC_NUMBER 0xAFFEAFFE
#define CTR_CONTAINER_VERSION 1
#define CTR_HEADER_SIZE 24
#define CTR_ADDR_SIZE 4
#define CTR_SYMBOL_NAME_SIZE 512
#define CTR_SYMBOL_SIZE 516

// address type
typedef unsigned int ctr_addr;

// container header type
typedef struct ctr_header {
    unsigned int magic_number;
    unsigned int container_version;
    unsigned int content_version;
    size_t symbol_segment_size;
    size_t data_segment_size;
    size_t text_segment_size;
} ctr_header_t;

// container segment type
typedef struct ctr_segment {
    char* data;
    size_t size;
} ctr_segment_t;

// basic read
ctr_header_t ctr_read_header(FILE* file);
ctr_segment_t ctr_read_symbol_segment(FILE* file, ctr_header_t header);
ctr_segment_t ctr_read_data_segment(FILE* file, ctr_header_t header);
ctr_segment_t ctr_read_text_segment(FILE* file, ctr_header_t header);

// basic write
void ctr_write_segment(FILE* file, ctr_segment_t symbol, ctr_segment_t data, ctr_segment_t text);

// symbol segment functions
unsigned int ctr_symbol_count(ctr_segment_t segment);
ctr_addr ctr_symbol_get_addr(ctr_segment_t segment, unsigned int index);
void ctr_symbol_set_addr(ctr_segment_t segment, unsigned int index, ctr_addr addr);
void ctr_symbol_get_name(ctr_segment_t segment, unsigned int index, char* name);
void ctr_symbol_set_name(ctr_segment_t segment, unsigned int index, char* name);
void ctr_symbol_resize_segment(ctr_segment_t* segment, unsigned int count);
int ctr_symbol_find_by_addr(ctr_segment_t segment, ctr_addr addr);

// text segment functions
unsigned int ctr_text_count(ctr_segment_t segment);
char ctr_text_get_instruction(ctr_segment_t segment, unsigned int index);
void ctr_text_set_instruction(ctr_segment_t segment, unsigned int index, char instruction);
void ctr_text_resize_segment(ctr_segment_t* segment, unsigned int count);

#endif
