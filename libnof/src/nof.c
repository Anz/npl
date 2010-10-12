#include "nof.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

nof_t* nof_create(size_t data_size, size_t text_size) {
    // alloc nof
    nof_t* nof = (nof_t*)malloc(sizeof(nof_t));
    if (nof == NULL) {
        return NULL;
    }

    // set header
    nof->file_version = NOF_FILE_VERSION;
    nof->bytecode_version = NOF_BYTECODE_VERSION;
    list_init(&nof->symbols, sizeof(nof_symbol_t));
    nof->data_size = data_size;
    nof->text_size = text_size;

    // alloc data
    nof->data = (char*)calloc(data_size, sizeof(char));
    if (nof->data == NULL) {
        list_release(&nof->symbols);
        free(nof);
        return NULL;
    }
    memset(nof->data, 0, data_size);

    // alloc text
    nof->text = (char*)calloc(text_size, sizeof(char));
    if (nof->text == NULL) {
        list_release(&nof->symbols);
        free(nof->data);
        free(nof);
        return NULL;
    }
    memset(nof->text, 0, text_size);

    return nof;
}

nof_t* nof_open(FILE* file) {
    // file size
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    if (file_size < NOF_HEADER_SIZE) {
        return NULL;
    }

    // load header
    char header[NOF_HEADER_SIZE];
    fread(header, sizeof(char), NOF_HEADER_SIZE, file);

    // check magic number
    int magic_number = swap_endian(*((int*)header));
    if (magic_number != NOF_MAGIC_NUMBER) {
        return NULL;
    }

    // init nof
    nof_t* nof = malloc(sizeof(nof_t));
    nof->file_version = swap_endian(((int*)header)[1]);
    nof->bytecode_version = swap_endian(((int*)header)[2]);
    size_t symbol_size = swap_endian(((int*)header)[3]);
    nof->data_size = swap_endian(((int*)header)[4]);
    nof->text_size = swap_endian(((int*)header)[5]);

    // check size
    size_t segment_size = NOF_HEADER_SIZE + symbol_size + nof->data_size + nof->text_size;
    if (file_size != segment_size) {
        return NULL;
    }

    // read symbols
    char symbols[symbol_size];
    fread(symbols, sizeof(char), symbol_size, file);


    // data init
    nof->data = malloc(nof->data_size);
    fread(nof->data, sizeof(char), nof->data_size, file);

    // text init
    nof->text = malloc(nof->text_size);
    fread(nof->text, sizeof(char), nof->text_size, file);

    // init symbols
    list_init(&nof->symbols, sizeof(nof_symbol_t));
    for(int i = 0; i+NOF_SYMBOL_NAME+4 <= symbol_size; i+=NOF_SYMBOL_NAME+4) {
        nof_symbol_t symbol;
        symbol.pointer = nof->text + swap_endian(*((int*)&symbols[i]));
        memcpy(symbol.name, &symbols[i+4], NOF_SYMBOL_NAME);
        list_add(&nof->symbols, &symbol);
    }

    return nof;
}

unsigned int nof_symbol_pointer_convert(nof_t* nof, void* pointer) {
    unsigned int addr = (unsigned int)pointer;
    addr -= (unsigned int)nof->text;
    return swap_endian(addr);
}

void nof_write(nof_t* nof, FILE* file) {
    // calculate symbol size
    size_t symbol_size = nof->symbols.count * (NOF_SYMBOL_NAME+4);

    // write header
    char header[NOF_HEADER_SIZE];
    memset(header, 0xBB, NOF_HEADER_SIZE);
    int* header_i = (int*)header;
    header_i[0] = swap_endian(NOF_MAGIC_NUMBER);
    header_i[1] = swap_endian(nof->file_version);
    header_i[2] = swap_endian(nof->bytecode_version);
    header_i[3] = swap_endian(symbol_size);
    header_i[4] = swap_endian(nof->data_size);
    header_i[5] = swap_endian(nof->text_size);
    fwrite(header, sizeof(char), NOF_HEADER_SIZE, file);

    // write symbol
    list_node iterator =  list_first(&nof->symbols);
    while (iterator != NULL) {
        nof_symbol_t* symbol = (nof_symbol_t*)list_get(iterator);
        //unsigned int pointer = swap_endian((unsigned int)(symbol->pointer - ((void*)nof->text)));
        //unsigned int pointer = swap_endian((unsigned int)(symbol->pointer));
        unsigned int pointer = nof_symbol_pointer_convert(nof, symbol->pointer);
        fwrite(&pointer, 4, 1, file);
        fwrite(symbol->name, sizeof(char), NOF_SYMBOL_NAME, file);
        iterator = list_next(iterator);
    }

    // write data
    fwrite(nof->data, sizeof(char), nof->data_size, file);

    // write text
    fwrite(nof->text, sizeof(char), nof->text_size, file);
}

void nof_close(nof_t* nof) {
    list_release(&nof->symbols);
    free(nof->data);
    free(nof->text);
    free(nof);
}

void nof_symbol_add(nof_t* nof, void* pointer, char* name) {
    nof_symbol_t symbol;
    memset(symbol.name, 0, NOF_SYMBOL_NAME+1);
    strncpy(symbol.name, name, NOF_SYMBOL_NAME);
    symbol.pointer = pointer;
    list_add(&nof->symbols, &symbol);
}
