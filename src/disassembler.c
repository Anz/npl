#include <stdio.h>
#include <stdlib.h>
#include "container.h"
#include "assembly.h"
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "map.h"
#include "list.h"

void print_usage();
void print_header(ctr_header_t header);
void print_symbols(ctr_t* container);
void print_externals(ctr_t* container);
void print_text(ctr_t* container);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }
    
    // parse arguments
    bool setheader = false, setsymbol = false, settext = false;
    for(int count = 1; count < argc-1; count++){
        if(strcmp(argv[count], "-i") == 0){
            setheader = true;
        }
        if(strcmp(argv[count], "-s") == 0){
            setsymbol = true;
        }
        if(strcmp(argv[count], "-t") == 0){
            settext = true;
        }
        if(strcmp(argv[count], "-h") == 0){
            print_usage();
            return 0;
        }
    }
    
    // load file
    char* path = argv[argc-1];
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

    // print header
    if(setheader){
        print_header(container.header);
    }

    // check magic number
    if (container.header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    if(setsymbol){
        print_symbols(&container);
    }
    
    //in the following paragraph is a stack smashing bug, but i can't find it :(
    // print external symbols
    if(setsymbol){
        print_externals(&container);
    }

    // print text
    if(settext){
        print_text(&container);
    }

    // release
    ctr_release(&container);
    asm_release();

    return 0;
}

void print_usage() {
    printf("usage: nis [options] <input>\n"
		"\t-i\tshow header\n"
		"\t-s\tshow symbol\n"
		"\t-t\tshow text\n");
}

void print_header(ctr_header_t header) {
    printf("header segment:\n\n");
    printf("%08X:\tmagic number 0x%X\n", 0, header.magic_number);
    printf("%08X:\tcontainer v%u\n", 4,  header.container_version);
    printf("%08X:\tcontent   v%u\n", 8, header.content_version);
    printf("%08X:\tsymbol    %4.2f KB (%u Bytes)\n", 12, header.symbol_size / 1024.0, header.symbol_size);
    printf("%08X:\texternal  %4.2f KB (%u Bytes)\n", 16, header.external_size / 1024.0, header.external_size);
    printf("%08X:\ttext      %4.2f KB (%u Bytes)\n", 20, header.text_size / 1024.0, header.text_size);
}

void print_symbols(ctr_t* container) {
    printf("symbol segment:\n\n");
    printf("%10s\taddr     \tname\n\n", "");
    list_t* symbols = &container->symbols.list;
    for(unsigned int index = 0; index < symbols->count; index++) {
        map_entry_t* symbol = list_get(symbols, index);
        printf("%08X:\t%08X \t%s\n", CTR_HEADER_SIZE + index * CTR_SYMBOL_SIZE, *(ctr_addr*)symbol->value, (char*)symbol->key);
    }
    printf("\n");
}

void print_externals(ctr_t* container) {
    printf("external symbol segment:\n\n");
    printf("%10s\tname\n\n", "");
    list_t* externals = &container->externals.list;
    size_t offset = CTR_HEADER_SIZE + container->header.symbol_size;
    for (unsigned int i = 0; i < externals->count; i++) {
        map_entry_t* external = list_get(externals, i);
        printf("%08X:\t%s\n", offset + i * CTR_SYMBOL_NAME_SIZE, (char*)external->key);
    }
    printf("\n");
}

void print_text(ctr_t* container) {
    printf("text segment:\n\n");
    printf("%11s\taddr      \tbinary        \tname\targument\n\n", "");
    size_t offset = CTR_HEADER_SIZE + container->header.symbol_size + container->header.external_size;
    list_t* texts = &container->texts;
    map_t* symbols = &container->symbols;
    map_t* externals = &container->externals;

    map_t variables;
    map_init(&variables, sizeof(char), sizeof(int));
    int variable_index = 0;

    for(unsigned int i = 0; i < texts->count; i++) {
        ctr_bytecode_t* bc = list_get(texts, i);

        char* symbol = (char*)map_find_value(symbols, &i);
        if (symbol) {
            printf("%08X <%s>:\n", i, symbol);
        }

        printf(" %08X:\t", offset + i * CTR_BYTECODE_SIZE);
        printf("%08X\t", i);
        printf("%02X ", char2int(bc->instruction));
        printf("%02X ", char2int(int2char(bc->argument, 0)));
        printf("%02X ", char2int(int2char(bc->argument, 1)));
        printf("%02X ", char2int(int2char(bc->argument, 2)));
        printf("%02X\t", char2int(int2char(bc->argument, 3)));
        printf("%s\t", asm_opcode2mnemonic(bc->instruction));

        switch (bc->instruction) {
            case ASM_JMP:
            case ASM_JE:
            case ASM_JNE:
            case ASM_JL:
            case ASM_JLE:
            case ASM_JG:
            case ASM_JGE:
            case ASM_CALL: {
                ctr_addr addr = i + bc->argument;
                char* symbol = (char*)map_find_value(symbols, &addr);
                if (symbol) {
                    printf("%s\n", symbol);
                } else {
                    printf("0x%08X\n", bc->argument);
                }
                break;
            }
            case ASM_CALLE: {
                int addr = bc->argument;
                char* symbol = (char*)map_find_value(externals, &addr);
                if (symbol) {
                    printf("%s\n", symbol);
                } else {
                    printf("0x%08X\n", bc->argument);
                }
                break;
            }
            case ASM_ENTER: {
                /*for (int i = 0; i < bc->argument / 4; i++) {
                    printf("%c, ", number2str(i));
                }*/
                variable_index = 0;
                map_release(&variables);
                map_init(&variables, sizeof(char), sizeof(int));
                printf("%i bytes\n", bc->argument);
                break;
            }
            case ASM_ARG: {
                char* variable = map_find_value(&variables, &bc->argument);
                if (!variable) {
                    char c = number2str(variable_index);
                    variable_index++;
                    map_set(&variables, &c, &bc->argument);
                    variable = map_find_value(&variables, &bc->argument);
                }
                printf("%c\n", *variable);
                break;
            }
            case ASM_ARGV: {
                printf("%i\n", bc->argument);
                break;
            }
            default: {
                printf("\n");
                break;
            }
        }
    }
    printf("\n");
}
