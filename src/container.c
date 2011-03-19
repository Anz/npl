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
    header.version = swap_endian(segment[1]); 
    header.symbol_size = swap_endian(segment[2]);
    header.data_size = swap_endian(segment[3]);
    header.text_size = swap_endian(segment[4]);

   return header;
}

// read symbol segment
void read_symbol(FILE* file, size_t size, ctr_t* container) {
    long data_segment = CTR_HEADER_SIZE + container->header.symbol_size;
    long text_segment = data_segment + container->header.data_size;

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
        symbol.data = NULL;

        long pointer = ftell(file);
        switch (type) {
             case CTR_SYMBOL_STR: {
                 fseek(file, data_segment + addr, SEEK_SET);
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
                 fseek(file, data_segment + addr, SEEK_SET);
                 symbol.data = malloc(type_size);
                 fread(symbol.data, 1, sizeof(int32_t), file);
                 int32_t* integer = symbol.data;
                 *integer = swap_endian(*integer);
                 break;
            }
            case CTR_SYMBOL_FUNC: {
                fseek(file, text_segment + addr, SEEK_SET);
                list_t* bytecodes = malloc(sizeof(list_t));
                list_init(bytecodes, sizeof(ctr_bytecode_t));
                for (int i = 0; i < type_size / 5; i++) {
                    char code[5];
                    fread(code, 5, sizeof(char), file);
                    ctr_bytecode_t bytecode;
                    bytecode.opcode = code[0];
                    bytecode.symbol = NULL;
                    list_add(bytecodes, &bytecode);
                }
                symbol.data = bytecodes;
                break;
            }
        }
        fseek(file, pointer, SEEK_SET);

        // add symbol
        map_set(&container->symbols, name, &symbol);
    }
}

// init container
void ctr_init(ctr_t* container) {
    container->header.data_size = 0;
    map_init(&container->symbols, MAP_STR, sizeof(ctr_symbol_t));
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

    read_symbol(file, c.header.symbol_size, &c);
    return c;
}

// write file
void ctr_write(FILE* file, ctr_t* container) {
    map_t* symbols = &container->symbols;

    // write header
    int32_t symbol_size = 0;
    int32_t data_size = 0;
    int32_t text_size = 0;
    for (int i = 0; i < symbols->list.count; i++) {
        map_entry_t* entry = list_get(&symbols->list, i);
        ctr_symbol_t* symbol = entry->value;

        symbol_size += CTR_NSYMBOL_SIZE + strlen(entry->key);

        switch (symbol->type) {
            case CTR_SYMBOL_INT: data_size += sizeof(int32_t); break;
            case CTR_SYMBOL_STR: data_size += sizeof(int32_t) * *(int32_t*)symbol->data; break;
            case CTR_SYMBOL_FUNC: text_size += ((list_t*)symbol->data)->count * 5; break;
        }
    }
    unsigned int header[] = {
        swap_endian(CTR_MAGIC_NUMBER),
        swap_endian(CTR_VERSION),
        swap_endian(symbol_size),
        swap_endian(data_size),
        swap_endian(text_size)
    };
    fwrite(header, sizeof(char), CTR_HEADER_SIZE, file);

    // write symbol
    int32_t data_index = 0;
    int32_t text_index = 0;
    for (int i = 0; i < symbols->list.count; i++) {
        map_entry_t* entry = list_get(&symbols->list, i);
        ctr_symbol_t* symbol = entry->value;
        int32_t size = 0;
        int32_t addr = 0;
        switch (symbol->type) {
            case CTR_SYMBOL_STR:
                addr = data_index;
                size = (*(int32_t*)symbol->data) * sizeof(int32_t);
                data_index += size;
                break;
            case CTR_SYMBOL_INT:
                addr = data_index;
                size = sizeof(int32_t);
                data_index += size;
                break;
            case CTR_SYMBOL_FUNC: 
                addr = text_index;
                size = ((list_t*)symbol->data)->count * 5;
                text_index += size;
                break;
        }
	int32_t length = strlen(entry->key);
	int32_t symbol_data[] = {
            swap_endian(addr),
            swap_endian(size),
            swap_endian(symbol->type),
            swap_endian(length)
        };
        fwrite(symbol_data, 4, sizeof(int32_t), file);
        fwrite(entry->key, length, sizeof(char), file);
    }

    // write data
    for (int i = 0; i < symbols->list.count; i++) {
        map_entry_t* entry = list_get(&symbols->list, i);
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
    for (int i = 0; i < symbols->list.count; i++) {
        map_entry_t* entry = list_get(&symbols->list, i);
        ctr_symbol_t* symbol = entry->value;

        if (symbol->type != CTR_SYMBOL_FUNC) {
            continue;
        }

        list_t* bytecodes = symbol->data;
        for (int i = 0; i < bytecodes->count; i++) {
            ctr_bytecode_t* bytecode = list_get(bytecodes, i);

            char instruction[] = { (char)bytecode->opcode, 0x0, 0x0, 0x0, 0x0 };
            fwrite(instruction, 5, sizeof(char), file);
        }
    }
}

// release container
void ctr_release(ctr_t* container) {
    map_release(&container->symbols);
}

