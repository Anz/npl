#include <stdio.h>
#include <stdlib.h>
#include <nof.h>

void print_usage();

int main(int argc, char* argv[]) {

    if (argc < 2) {
        print_usage();
        return 0;
    }

    // load file
    char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");

    // on error, print error
    if (file == NULL) {
        printf("could not open file: %s\n", file_path);
        return 1;
    }

    // load nof
    nof_t* nof = nof_open(file);
    fclose(file);
    if (nof == NULL) {
        printf("file format wrong: %s\n", file_path);
        return 1;
    }

    // print infos
    printf("file version: %u\n", nof->file_version);
    printf("bytecode version: %u\n", nof->bytecode_version);
    printf("symbol count: %u\n", nof->symbols.count);
    printf("data size: %u\n", nof->data_size);
    printf("text size: %u\n", nof->text_size);

    // print symbols
    list_node iterator = list_first(&nof->symbols);
    while (iterator != NULL) {
        nof_symbol_t* symbol = (nof_symbol_t*)list_get(iterator);
        printf("symbol %08x: \"%s\" \n", symbol->pointer, symbol->name);
        iterator = list_next(iterator);
    }

    // print data/text
    nof->data[nof->data_size-1] = 0;
    printf("data: \"%s\" \n", nof->data);
    for (int i = 0; i < nof->text_size; i++) {
        char op = nof->text[i];
        printf("text: 0x%02X \n", op);
    }

    nof_close(nof);

    printf("Hello world!\n");
    return 0;
}

void print_usage() {
    printf("usage\n");
}
