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
    header.nsymbol_size = swap_endian(segment[4]);
    header.external_size = swap_endian(segment[5]);
    header.data_size = swap_endian(segment[6]);
    header.text_size = swap_endian(segment[7]);

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

// read nsymbol segment
void read_nsymbol(FILE* file, size_t size, ctr_t* container) {
    for (size_t i = 0; i < size; i += 4 * sizeof(int32_t)) {
        // read
        int32_t buffer[4];
        fread(buffer, 4, sizeof(int32_t), file);
        int32_t addr = swap_endian(buffer[0]);
        int32_t type_size = swap_endian(buffer[1]);
        int32_t type = swap_endian(buffer[2]);
        int32_t length = swap_endian(buffer[3]);
        char* name = malloc(length + 1);
        fread(name, length, sizeof(char), file);
        name[length] = '\0';
        i += length;

        // create symbol
        ctr_symbol_t symbol;
        symbol.type = type;

        long pointer = ftell(file);
        long data_segment = CTR_HEADER_SIZE + container->header.symbol_size + container->header.nsymbol_size  + container->header.external_size;
        fseek(file, data_segment + addr, SEEK_SET);
        switch (type) {
             case CTR_SYMBOL_STR: {
                 symbol.data = malloc(type_size + sizeof(int32_t));
                 int32_t length = type_size / sizeof(int32_t);
                 fread(symbol.data + sizeof(int32_t), type_size, 1, file);
                 int32_t* string = symbol.data;
                 for (int32_t i = 0; i < length; i++) {
                     string[i + 1] = swap_endian(string[i + 1]);
                 }
                 string[0] = length;
                 break;
            }
            case CTR_SYMBOL_INT: {
                 symbol.data = malloc(type_size);
                 fread(symbol.data, 1, sizeof(int32_t), file);
                 int32_t* integer = symbol.data;
                 *integer = swap_endian(*integer);
                 break;
            } 
        }
        fseek(file, pointer, SEEK_SET);

        // add symbol
        map_set(&container->nsymbols, name, &symbol);
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

// read data segment
static void read_data_segment(FILE* file, size_t size) {
    void* data = malloc(size);
    fread(data, size, sizeof(char), file);
}

// read text segment
void read_texts(FILE* file, size_t size, list_t* texts) {
    for (int i = 0; i < size / CTR_BYTECODE_SIZE; i++) {
        // read
        char buffer[CTR_BYTECODE_SIZE];
        fread(buffer, CTR_BYTECODE_SIZE, 1, file);

        // fill data
        ctr_bytecode_t bc;
        bc.instruction = buffer[0];
        bc.argument = swap_endian(*(int*)&buffer[1]);
        
        list_add(texts, &bc);
   }
}

// init container
void ctr_init(ctr_t* container) {
    container->header.data_size = 0;
    map_init(&container->symbols, MAP_STR, sizeof(ctr_addr));
    map_init(&container->nsymbols, MAP_STR, sizeof(ctr_symbol_t));
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
    read_nsymbol(file, c.header.nsymbol_size, &c);
    read_externals(file, c.header.external_size, &c.externals);
    read_data_segment(file, c.header.data_size);
    read_texts(file, c.header.text_size, &c.texts);
    return c;
}

// write file
void ctr_write(FILE* file, ctr_t* container) {
    map_t* symbols = &container->symbols;
    map_t* nsymbols = &container->nsymbols;
    map_t* externals = &container->externals;
    list_t* texts = &container->texts;

    // write header
    int symbol_size = symbols->list.count * CTR_SYMBOL_SIZE;
    int nsymbol_size = 0;
    for (int i = 0; i < nsymbols->list.count; i++) {
        map_entry_t* entry = list_get(&nsymbols->list, i);
        ctr_symbol_t* symbol = entry->value;
        nsymbol_size += CTR_NSYMBOL_SIZE + strlen(entry->key);
    }
    int external_size = externals->list.count * CTR_SYMBOL_NAME_SIZE;
    int data_size = container->header.data_size;
    int text_size = texts->count * CTR_BYTECODE_SIZE;
    unsigned int header[] = {
        swap_endian(CTR_MAGIC_NUMBER),
        swap_endian(CTR_CONTAINER_VERSION),
        swap_endian(CTR_BYTECODE_VERSION),
        swap_endian(symbol_size),
        swap_endian(nsymbol_size),
        swap_endian(external_size),
        swap_endian(data_size),
        swap_endian(text_size)
    };
    fwrite(header, sizeof(char), CTR_HEADER_SIZE, file);

    // write symbol
    for (unsigned int i = 0; i < symbols->list.count; i++) {
        map_entry_t* entry = list_get(&symbols->list, i);
        ctr_addr addr = swap_endian(*(int*)entry->value);
        fwrite(&addr, sizeof(char), CTR_ADDR_SIZE, file);
        fwrite(entry->key, sizeof(char), CTR_SYMBOL_NAME_SIZE, file);
    }

    // write symbol
    int32_t data_index = 0;
    for (int i = 0; i < nsymbols->list.count; i++) {
        map_entry_t* entry = list_get(&nsymbols->list, i);
        ctr_symbol_t* symbol = entry->value;
        int32_t size = 0;
        switch (symbol->type) {
            case CTR_SYMBOL_STR: size = (*(int32_t*)symbol->data) * sizeof(int32_t); break;
            case CTR_SYMBOL_INT: size = sizeof(int32_t); break;
        }
	int32_t length = strlen(entry->key);
	int32_t symbol_data[] = {
            swap_endian(data_index),
            swap_endian(size),
            swap_endian(symbol->type),
            swap_endian(length)
        };
        fwrite(symbol_data, 4, sizeof(int32_t), file);
        fwrite(entry->key, length, sizeof(char), file);
        data_index += size;
    }

    // write external symbol
    for (unsigned int i = 0; i < externals->list.count; i++) {
        map_entry_t* entry = list_get(&externals->list, i);
        fwrite(entry->key, sizeof(char), CTR_SYMBOL_NAME_SIZE, file);
    }

    // write data
    for (int i = 0; i < nsymbols->list.count; i++) {
        map_entry_t* entry = list_get(&nsymbols->list, i);
        ctr_symbol_t* symbol = entry->value;
        switch (symbol->type) {
            case CTR_SYMBOL_STR: {
                int32_t* string = symbol->data;
                int32_t length = string[0];
                printf("write strlen: %i\n", length);
                for (int i = 0; i < length; i++) {
                    int32_t value = swap_endian(string[i + 1]);
                    fwrite(&value, 1, sizeof(int32_t), file);
                }
                break;
            }
            case CTR_SYMBOL_INT: {
                int32_t value = swap_endian(*(int32_t*)symbol->data);
                fwrite(&value, 1, sizeof(int32_t), file);
                break;
            }
        }
    }



    // write text
    for (unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);
        fwrite(&bc->instruction, sizeof(char), 1, file);
        int arg = swap_endian(bc->argument);
        fwrite(&arg, sizeof(int), 1, file);
    }

}

// release container
void ctr_release(ctr_t* container) {
    map_release(&container->symbols);
    map_release(&container->externals);
    list_release(&container->texts);
}

