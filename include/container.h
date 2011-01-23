#ifndef CTR_H
#define CTR_H

#include <stdio.h>
#include "map.h"

// constans
#define CTR_MAGIC_NUMBER 0xAFFEAFFE
#define CTR_CONTAINER_VERSION 1
#define CTR_BYTECODE_VERSION 1
#define CTR_HEADER_SIZE 24
#define CTR_ADDR_SIZE 4
#define CTR_SYMBOL_NAME_SIZE 512
#define CTR_SYMBOL_SIZE 516
#define CTR_BYTECODE_SIZE 5

// address type
typedef int ctr_addr;

// container header type
typedef struct ctr_header {
    unsigned int magic_number;
    unsigned int container_version;
    unsigned int content_version;
    size_t symbol_size;
    size_t external_size;
    size_t text_size;
} ctr_header_t;

// container type
typedef struct ctr {
    ctr_header_t header;
    map_t symbols;
    map_t externals;
    list_t texts;
} ctr_t;

// instruction
typedef struct ctr_bytecode {
    char instruction;
    int argument;
} ctr_bytecode_t;

// init container
void ctr_init(ctr_t* container);
// read file
ctr_t ctr_read(FILE* file);
// write file
void ctr_write(FILE* file, ctr_t* container);
// release container
void ctr_release(ctr_t* container);

#endif
