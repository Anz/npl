#include "bytecode.h"
#include "container.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

// read header
ctr_header_t read_header(FILE* file) {
    ctr_header_t header;

    // segment
    unsigned int segment[CTR_HEADER_SIZE / sizeof(int)];
    fseek(file, 0, SEEK_SET);
    fread(segment, sizeof(char), CTR_HEADER_SIZE, file);

    // init header
    header.magic_number = swap_endian(segment[0]);
    header.container_version = swap_endian(segment[1]); 
    header.content_version = swap_endian(segment[2]);
    header.symbol_size = swap_endian(segment[3]);
    header.external_size = swap_endian(segment[4]);
    header.text_size = swap_endian(segment[5]);

   return header;
}

// read symbol segment
void read_symbols(FILE* file, size_t size, map_t* symbols) {
    for (int i = 0; i < size / CTR_SYMBOL_SIZE; i++) {
        // read
        char buffer[CTR_SYMBOL_SIZE];
        fread(buffer, CTR_SYMBOL_SIZE, 1, file);

        // fill data
        ctr_addr addr = swap_endian(*(int*)buffer);
        char name[CTR_SYMBOL_NAME_SIZE+1];
        memcpy(name, &buffer[4], CTR_SYMBOL_NAME_SIZE);
        name[CTR_SYMBOL_NAME_SIZE] = '\0';

        map_set(symbols, name, &addr);
    }
}

// read external segment
void read_externals(FILE* file, size_t size, map_t* externals) {
    for (int i = 0; i < size / CTR_SYMBOL_NAME_SIZE; i++) {
        // read
        char name[CTR_SYMBOL_NAME_SIZE+1];
        fread(name, CTR_SYMBOL_NAME_SIZE, 1, file);
        name[CTR_SYMBOL_NAME_SIZE+1] = '\0';
        int index = i;
        map_set(externals, name, &index);
    }
}

// read text segment
void read_texts(FILE* file, size_t size, list_t* texts) {
    for (int i = 0; i < size / BC_OPCODE_SIZE; i++) {
        // read
        char buffer[BC_OPCODE_SIZE];
        fread(buffer, BC_OPCODE_SIZE, 1, file);

        // fill data
        ctr_bytecode_t bc;
        bc.instruction = buffer[0];
        bc.argument = swap_endian(*(int*)&buffer[1]);
        
        list_add(texts, &bc);
   }
}

// init container
void ctr_init(ctr_t* container) {
    map_init(&container->symbols, MAP_STR, sizeof(ctr_addr));
    map_init(&container->externals, MAP_STR, sizeof(int));
    list_init(&container->texts, sizeof(ctr_bytecode_t));
}

// read file
ctr_t ctr_read(FILE* file) {
    ctr_t c;
    ctr_init(&c);

    c.header = read_header(file);

    // check magic number
    if (c.header.magic_number != CTR_MAGIC_NUMBER) {
        return c;
    }

    read_symbols(file, c.header.symbol_size, &c.symbols);
    read_externals(file, c.header.external_size, &c.externals);
    read_texts(file, c.header.text_size, &c.texts);
    return c;
}

// write file
void ctr_write(FILE* file, ctr_t* container) {
}

// release container
void ctr_release(ctr_t* container) {
    map_release(&container->symbols);
    map_release(&container->externals);
    list_release(&container->texts);
}

