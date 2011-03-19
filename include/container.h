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

// container header type
typedef struct ctr_header {
    int32_t magic_number;
    int32_t version;
    int32_t symbol_size;
    int32_t data_size;
    int32_t text_size;
} ctr_header_t;

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

// read file
ctr_header_t ctr_read_header(FILE* file);
void ctr_read_symbols(FILE* file, map_t* symbols);

// write file
void ctr_write(FILE* file, map_t* symbols);

#endif
