#ifndef NOF_H
#define NOF_H

//#include "list.h"
#include <stddef.h>
#include <stdio.h>

// constans
#define NOF_MAGIC_NUMBER 0xAFFEAFFE
#define NOF_CONTAINER_VERSION 1
#define NOF_HEADER_SIZE 24
#define NOF_ADDR_SIZE 4
#define NOF_SYMBOL_NAME_SIZE 512
#define NOF_SYMBOL_SIZE 516

#define NOF_MAGIC_NUMBER_INDEX 0
#define NOF_CONTAINER_VERSION_INDEX 4
#define NOF_CONTENT_VERSION_INDEX 8
#define NOF_SYMBOL_SIZE_INDEX 12
#define NOF_DATA_SIZE_INDEX 16
#define NOF_TEXT_SIZE_INDEX 20

typedef unsigned int ctr_addr;

typedef struct nof_header {
    unsigned int magic_number;
    unsigned int container_version;
    unsigned int content_version;
    size_t symbol_segment_size;
    size_t data_segment_size;
    size_t text_segment_size;
} nof_header_t;

// basic read
nof_header_t nof_read_header(FILE* file);
char* nof_read_symbol_segment(FILE* file, nof_header_t header);
char* nof_read_data_segment(FILE* file, nof_header_t header);
char* nof_read_text_segment(FILE* file, nof_header_t header);

// basic write
void nof_write_segment(FILE* file, nof_header_t, char* symbol, char* data, char* text);

// symbol segment functions
unsigned int nof_symbol_count(nof_header_t header);
ctr_addr nof_symbol_get_addr(char* segment, unsigned int index);
void nof_symbol_set_addr(char* segment, unsigned int index, ctr_addr addr);
void nof_symbol_get_name(char* segment, unsigned int index, char* name);
void nof_symbol_set_name(char* segment, unsigned int index, char* name);
void nof_symbol_resize_segment(char** segment, unsigned int count);

// text segment functions
unsigned int nof_text_count(nof_header_t header);
char nof_text_get_instruction(char* segment, unsigned int index);
void nof_text_set_instruction(char* segment, unsigned int index, char instruction);
void nof_text_resize_segment(char** segment, unsigned int count);

#endif
