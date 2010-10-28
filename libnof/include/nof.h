#ifndef NOF_H
#define NOF_H

#include "list.h"
#include <stddef.h>
#include <stdio.h>

// constans
#define NOF_MAGIC_NUMBER 0xAFFEAFFE
#define NOF_CONTAINER_VERSION 1
#define NOF_HEADER_SIZE 24
#define NOF_ADDR_SIZE 4

#define NOF_MAGIC_NUMBER_INDEX 0
#define NOF_CONTAINER_VERSION_INDEX 4
#define NOF_CONTENT_VERSION_INDEX 8
#define NOF_SYMBOL_SIZE_INDEX 12
#define NOF_DATA_SIZE_INDEX 16
#define NOF_TEXT_SIZE_INDEX 20

typedef struct nof_header {
    unsigned int magic_number;
    unsigned int container_version;
    unsigned int content_version;
    size_t symbol_segment_size;
    size_t data_segment_size;
    size_t text_segment_size;
} nof_header_t;

typedef struct nof_symbol {
    unsigned int pointer;
    char* name;
} nof_symbol_t;

// basic read
nof_header_t nof_read_header(FILE* file);
char* nof_read_symbol_segment(FILE* file, nof_header_t header);
char* nof_read_data_segment(FILE* file, nof_header_t header);
char* nof_read_text_segment(FILE* file, nof_header_t header);

// advanced read
list_t nof_read_symbols(char* segment, size_t size);

// parse symbol segment
int nof_read_symbol(char* segment, size_t size, nof_symbol_t* symbol);

// write symbol segment
void nof_write_symbol(char* segment, nof_symbol_t symbol);

// basic write
void nof_write_segment(FILE* file, nof_header_t, char* symbol, char* data, char* text);

// advanced write
char* nof_write_to_buffer(list_t symbols, size_t* size);

#endif
