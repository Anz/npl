#include <stdio.h>
#include <stdlib.h>
#include "container.h"
#include "assembly.h"
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "map.h"
#include "list.h"

typedef enum modus {
    VERSION, INFO, LIST_SYM, SHOW_SYM
} modus_t;

void print_usage();
void print_version();
void print_info(ctr_t* container);
void print_all_symbols(map_t* symbols);
void print_symbol(char* name, ctr_symbol_t* symbol);

char* type_to_str(int32_t type) {
    static char buffer[10];
    switch (type) {
        case CTR_SYMBOL_STR: memcpy(buffer, "String", 7); break;
        case CTR_SYMBOL_INT: memcpy(buffer, "Integer", 8); break;
        case CTR_SYMBOL_FUNC: memcpy(buffer, "Func", 5); break;
        case CTR_SYMBOL_EXTERN: memcpy(buffer, "Extern", 7); break;
        default: memcpy(buffer, "Unkown", 7); break;
    }
    return buffer;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        print_usage();
        return 0;
    }
    
    // parse arguments
    modus_t modus;
    char* arg = argv[1];
    if (strcmp(arg, "-v") == 0)
        modus = VERSION;
    else if (strcmp(arg, "-i") == 0)
        modus = INFO;
    else if (strcmp(arg, "-l") == 0)
        modus = LIST_SYM;
    else if (memcmp(arg, "-s", 2) == 0)
        modus = SHOW_SYM;
    else {
        print_usage();
        return 0;
    }
    
    // load file
    char* path = argv[2];
    FILE* file = fopen(path, "rb");

    // on error, print error
    if (!file) {
        printf("could not open file: %s\n", path);
        return 1;
    }

    // init assembly table
    asm_init();

    // load container
    ctr_t container = ctr_read(file);
    fclose(file);

    // check magic number
    if (container.header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }

    switch (modus) {
        case VERSION: print_version(); break;
        case INFO: print_info(&container); break;
        case LIST_SYM: print_all_symbols(&container.symbols); break;
        case SHOW_SYM: print_symbol(&arg[2], map_find_key(&container.symbols, &arg[2])); break;
    }
    printf("\n");

    // release
    ctr_release(&container);
    asm_release();

    return 0;
}

void print_usage() {
    printf("usage: nis <option> <input>\n"
		"\t-h\thelp\n"
		"\t-v\tversion\n"
		"\t-i\tshow file information\n"
		"\t-l\tlist all symbols\n"
		"\t-s<name>\tshow symbol\n");
}

void print_version() {
    printf("NPL Disassembler\n");
}

void print_info(ctr_t* container) {
    ctr_header_t header = container->header;
    printf("MAGIC NUMBER\t0x%X\n", header.magic_number);
    printf("Version\t\t%u\n", header.version);
    printf("Symbol\t\t%4.2f KB (%u Bytes)\n", header.symbol_size / 1024.0, header.symbol_size);
    printf("Data\t\t%4.2f KB (%u Bytes)\n", header.data_size / 1024.0, header.data_size);
    printf("Text\t\t%4.2f KB (%u Bytes)\n", header.text_size / 1024.0, header.text_size);
}

void print_all_symbols(map_t* symbols) {
    printf("Type\t\tName\n\n");
    list_t* list = &symbols->list;
    for(unsigned int index = 0; index < list->count; index++) {
        map_entry_t* entry = list_get(list, index);
        char* name = entry->key;
        ctr_symbol_t* symbol = entry->value;
        printf("%s\t\t%s\n", type_to_str(symbol->type), name);
    }
}

void print_symbol(char* name, ctr_symbol_t* symbol) {
    if (!symbol) {
        fprintf(stderr, "symbol '%s' not found", name);
        return;
    }

    switch (symbol->type) {
        case CTR_SYMBOL_INT: {
            int32_t value = *(int32_t*)symbol->data;
            printf("%s\t\t%s\t%i\n", type_to_str(symbol->type), name, value);
            break;
        }
        case CTR_SYMBOL_STR: {
            int32_t length = *(int32_t*)symbol->data;
            char str[length + 1];
            for (int32_t i = 0; i < length; i++) {
                str[i] = (char)((int32_t*)symbol->data)[i + 1];
            }
            str[length] = '\0';
            printf("%s\t\t%s\t'%s'\n", type_to_str(symbol->type), name, str);
            break;
        }
        case CTR_SYMBOL_FUNC: {
            printf("@%s\n", name);
            list_t* bytecodes = symbol->data;
            for (int i = 0; i < bytecodes->count; i++) {
                ctr_bytecode_t* bytecode = list_get(bytecodes, i);
                printf("\t%s\n", asm_opcode2mnemonic(bytecode->opcode));
            }
            break;
        }
    }
}
