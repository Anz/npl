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
typedef int ctr_addr;

// container header type
typedef struct ctr_header {
    unsigned int magic_number;
    unsigned int container_version;
    unsigned int content_version;
    size_t symbol_segment_size;
    size_t external_symbol_segment_size;
    size_t text_segment_size;
} ctr_header_t;

// symbol
typedef struct ctr_symbol {
    char name[CTR_SYMBOL_NAME_SIZE+1];
    ctr_addr addr;
} ctr_symbol_t;

// external symbol
typedef struct ctr_external_symbol {
    char name[CTR_SYMBOL_NAME_SIZE+1];
    ctr_addr addr;
} ctr_external_symbol_t;


// basic read
ctr_header_t ctr_read_header(FILE* file);

// symbol segment functions
ctr_symbol_t* ctr_symbol_read(FILE* stream, ctr_header_t header);
int ctr_symbol_find(ctr_symbol_t* symbols, ctr_header_t header, ctr_addr addr);

#endif
