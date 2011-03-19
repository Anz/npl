#ifndef CTR_H
#define CTR_H

#include <stdio.h>
#include <stdint.h>
#include "map.h"

// constans
#define CTR_MAGIC_NUMBER 0xAFFEAFFE
#define CTR_VERSION 1
#define CTR_HEADER_SIZE 20
#define CTR_ADDR_SIZE 4
#define CTR_NSYMBOL_SIZE 16
#define CTR_BYTECODE_SIZE 5

#define CTR_SYMBOL_TYPE 0x0
#define CTR_SYMBOL_STR  0x1
#define CTR_SYMBOL_INT  0x2
#define CTR_SYMBOL_FUNC 0x3
#define CTR_SYMBOL_EXTERN 0x4

// address type
typedef int ctr_addr;

// container header type
typedef struct ctr_header {
    unsigned int magic_number;
    unsigned int version;
    size_t symbol_size;
    size_t data_size;
    size_t text_size;
} ctr_header_t;

// container type
typedef struct ctr {
    ctr_header_t header;
    map_t symbols;
} ctr_t;

// symbol
typedef struct ctr_symbol {
    int32_t type;
    void* data;
} ctr_symbol_t;

// instruction
typedef struct ctr_bytecode {
    char opcode;
    ctr_symbol_t* symbol;
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
