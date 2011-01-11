#include <stdio.h>
#include <stdlib.h>
#include <container.h>
#include <bytecode.h>
#include "util.h"
#include "map.h"
#include "list.h"

void print_usage();
list_t collect_external_symbols(FILE* file, ctr_header_t header);
map_t collect_symbols(FILE* file, ctr_header_t header);
void print_header(ctr_header_t header);
void print_symbols(FILE* file, ctr_header_t header);
void print_external_symbol_segment(list_t external_symbol, ctr_header_t header);
void print_text(FILE* file, ctr_header_t header, map_t* symbols, list_t* external_symbols);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }
    
    int count = 1;
    int setheader = 0, setsymbol = 0, settext = 0;
    for(count; count < argc; count++){
        if(strcmp(argv[count], "-i") == 0){
            setheader = 1;
        };
        if(strcmp(argv[count], "-s") == 0){
            setsymbol = 1;
        }
        if(strcmp(argv[count], "-t") == 0){
            settext = 1;
        }
    }
    
	//temp
	printf("setheader = %i\n", setheader);
	printf("setsymbol = %i\n", setsymbol);
	printf("settext = %i\n", settext);
	

    // load file
    char* path = argv[--count];
    FILE* file = fopen(path, "rb");

    // on error, print error
    if (file == NULL) {
        printf("could not open file: %s\n", path);
        return 1;
    }

    // load ctr
    ctr_header_t header = ctr_read_header(file);

    // print header
    if(setheader == 1){
        print_header(header);
    };

    // check magic number
    if (header.magic_number != CTR_MAGIC_NUMBER) {
        fprintf(stderr, "magic number is not equal to 0x%X - abort\n", CTR_MAGIC_NUMBER);
        return 1;
    }
    printf("\n");

    // print symbols
    map_t symbols = collect_symbols(file, header);
    if(setsymbol == 1){
        print_symbols(file, header);
    };
 
    // print external symbols
    list_t external_symbols = collect_external_symbols(file, header);
    if(setsymbol == 1){
        print_external_symbol_segment(external_symbols, header);
    }

    // print text
    if(settext == 1){
        print_text(file, header, &symbols, &external_symbols);
    }

    // release
    list_release(&external_symbols);
    map_release(&symbols);

    fclose(file);
    return 0;
}

void print_usage() {
    printf("usage: nis [options] <input>\n"
		"\t-i\tshow header\n"
		"\t-s\tshow symbol\n"
		"\t-t\tshow text\n");
}

map_t collect_symbols(FILE* file, ctr_header_t header) {
    map_t symbols;
    map_init(&symbols, CTR_SYMBOL_NAME_SIZE+1);
    unsigned int symbol_count = ctr_symbol_count(header);
    for(unsigned int index = 0; index < symbol_count; index++) {
        ctr_symbol_t symbol = ctr_symbol_read(file, index);
        map_addi(&symbols, symbol.addr, symbol.name);
    }
    return symbols;
}

list_t collect_external_symbols(FILE* file, ctr_header_t header) {
    list_t symbols;
    list_init(&symbols, sizeof(ctr_external_symbol_t));

    unsigned int count = ctr_external_count(header);
    for (unsigned int i = 0; i < count; i++) {
        ctr_external_symbol_t symbol = ctr_external_read(file, header, i);
        list_add(&symbols, symbol.name);
    };
    return symbols;
}

void print_header(ctr_header_t header) {
    printf("header segment:\n\n");
    printf("%08X:\tmagic number 0x%X\n", 0, header.magic_number);
    printf("%08X:\tcontainer v%u\n", 4,  header.container_version);
    printf("%08X:\tcontent   v%u\n", 8, header.content_version);
    printf("%08X:\tsymbol    %4.2f KB (%u Bytes)\n", 12, header.symbol_segment_size / 1024.0, header.symbol_segment_size);
    printf("%08X:\texternal  %4.2f KB (%u Bytes)\n", 16, header.external_symbol_segment_size / 1024.0, header.external_symbol_segment_size);
    printf("%08X:\ttext      %4.2f KB (%u Bytes)\n", 20, header.text_segment_size / 1024.0, header.text_segment_size);
}

void print_symbols(FILE* file, ctr_header_t header) {
    printf("symbol segment:\n\n");
    printf("%10s\taddr     \tname\n\n", "");
    unsigned int symbol_count = ctr_symbol_count(header);
    for(unsigned int index = 0; index < symbol_count; index++) {
        ctr_symbol_t symbol = ctr_symbol_read(file, index);
        printf("%08X:\t%08X \t%s\n", CTR_HEADER_SIZE + index * CTR_SYMBOL_SIZE, symbol.addr, symbol.name);
    }
    printf("\n");
}

void print_external_symbol_segment(list_t external_symbol, ctr_header_t header) {
    size_t offset = ctr_external_offset(header);

    printf("external symbol segment:\n\n");
    printf("%10s\tname\n\n", "");

    list_node symbol = list_first(&external_symbol);
    int i = 0;
    while (symbol != NULL) {
        char* name = list_data(symbol);
        printf("%08X:\t%s\n", offset + i, name);
        symbol = list_next(symbol);
        i += CTR_SYMBOL_NAME_SIZE;
    }
    printf("\n");
}

void print_text(FILE* file, ctr_header_t header, map_t* symbols, list_t* external_symbols) {
    printf("text segment:\n\n");
    printf("%11s\taddr      \tbinary        \tname\targument\n\n", "");
    unsigned int text_count = ctr_text_count(header);
    int offset = ctr_text_offset(header);
    for(unsigned int i = 0; i < text_count; i++) {
        ctr_bytecode_t bc = ctr_text_read(file, header, i);

        map_node_t* symbol = map_findi(symbols, i);
        if (symbol != NULL) {
            printf("%08X <%s>:\n", i, (char*)symbol->value);
        }

        printf(" %08X:\t", offset + i * BC_OPCODE_SIZE);
        printf("%08X\t", i);
        printf("%02X ", char2int(bc.instruction));
        printf("%02X ", char2int(int2char(bc.argument, 0)));
        printf("%02X ", char2int(int2char(bc.argument, 1)));
        printf("%02X ", char2int(int2char(bc.argument, 2)));
        printf("%02X\t", char2int(int2char(bc.argument, 3)));
        printf("%s\t", bc_op2asm(bc.instruction));

        switch (bc.instruction) {
            case BC_SYNC:
            case BC_ASYNC: {
                map_node_t* symbol_node = map_findi(symbols, i + bc.argument);
                if (symbol_node != NULL) {
                    printf("%s\n", (char*)symbol_node->value);
                } else {
                    printf("0x%08X\n", bc.argument);
                }
                break;
            }
            case BC_SYNCE:
            case BC_ASYNCE: {
                list_node symbol = list_get(external_symbols, bc.argument);
                if (symbol != NULL) {
                    printf("%s\n", (char*)list_data(symbol));
                } else {
                    printf("0x%08X\n", bc.argument);
                }
                break;
            }
            case BC_ENTER: {
                for (int i = 0; i < bc.argument / 4; i++) {
                    printf("%c, ", number2str(i));
                }
                printf("\n");
                break;
            }
            case BC_ARG: {
                printf("%c\n", number2str(bc.argument));
                break;
            }
            case BC_ARGV: {
                printf("%i\n", bc.argument);
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
