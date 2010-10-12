#ifndef NOF_H
#define NOF_H

#include "list.h"
#include <stddef.h>
#include <stdio.h>

// constans
#define NOF_MAGIC_NUMBER 0xAFFEAFFE
#define NOF_FILE_VERSION 1
#define NOF_BYTECODE_VERSION 2
#define NOF_HEADER_SIZE 32
#define NOF_SYMBOL_NAME 60

typedef struct nof_symbol {
    void* pointer;
    char name[NOF_SYMBOL_NAME+1];
} nof_symbol_t;

typedef struct nof {
    unsigned int file_version;
    unsigned int bytecode_version;
    list_t symbols;
    char* data;
    size_t data_size;
    char* text;
    size_t text_size;

} nof_t;

nof_t* nof_create(size_t data_size, size_t text_size);
nof_t* nof_open(FILE* file);
void nof_write(nof_t* nof, FILE* file);
void nof_close(nof_t* nof);
void nof_symbol_add(nof_t* nof, void* pointer, char* name);

#endif
