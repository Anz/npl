#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nof.h>
#include "bytecode.h"
#include "assembler.h"



void print_usage();
size_t get_file_size(FILE* file);

int main(int argc, char* argv[]) {

    if (argc < 3) {
        print_usage();
        return 0;
    }

    // load assembly
    char* assembly_path = argv[1];
    FILE* assembly = fopen(assembly_path, "r");
    if (assembly == NULL) {
        printf("file format wrong: %s\n", assembly_path);
        return 1;
        }

    // load bytecode
    char* bytecode_path = argv[2];
    FILE* bytecode = fopen(bytecode_path, "w");
    if (bytecode == NULL) {
        printf("file format wrong: %s\n", bytecode_path);
        return 1;
    }

    // get assembly file size
    size_t assembly_size = get_file_size(assembly);

    // load stuff and write
    char* assembly_content = malloc(assembly_size+1);
    fread(assembly_content, sizeof(char), assembly_size, assembly);
    fclose(assembly);
    assembly = NULL;
    int opcodes_count = 1;
    for (int i = 0; i < assembly_size; i++) {
        if (assembly_content[i] == '\n') {
            opcodes_count++;
        }
    }
    //assembly_content[assembly_size] = '\0';

    // init nof
    nof_t* nof = nof_create(16, opcodes_count);
    if (nof == NULL) {
        printf("fatal error: could not alloc nof_t memory");
    }

    strcpy(nof->data, "1234567890123456");

    // write text stuff
    assembler(nof, assembly_content, assembly_size);

    nof_write(nof,bytecode);
    fclose(bytecode);
    bytecode = NULL;

    size_t symbol_size = nof->symbols.count * (NOF_SYMBOL_NAME+4);
    printf("symbol size: %i\n", symbol_size);
    printf("data size: %i\n", nof->data_size);
    printf("text size: %i\n", nof->text_size);
    printf("total size: %i\n", NOF_HEADER_SIZE+symbol_size+nof->data_size+nof->text_size);

    nof_close(nof);
    nof = NULL;
    printf("finished\n");
    return 0;
}

void print_usage() {
    printf("usage\n");
}

size_t get_file_size(FILE* file) {
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return file_size;
}
