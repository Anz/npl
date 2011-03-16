#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "map.h"
#include "assembly.h"
#include "container.h"

#define SEG_DATA 0
#define SEG_TEXT 1
#define BUFFER_SIZE 512

static ctr_t read(FILE* file);
static char* func_name(char* line);
static ctr_bytecode_t assemble(char* mnemonic, char* arg1, char* arg2, void* symbols);
static ctr_symbol_t assemble_data(char* type, char* value);

int main(int argc, char* argv[]) {

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "wb");

    // read file
    asm_init();
    ctr_t container = read(input);
    fclose(input);

    // print
    list_t* symbols = &container.nsymbols.list;
    for (int i = 0; i < symbols->count; i++) {
        map_entry_t* entry = list_get(symbols, i);
        ctr_symbol_t* symbol = entry->value;
        printf("%s\n", (char*)entry->key);
        switch (symbol->type) {
            case CTR_SYMBOL_FUNC: {
                list_t* bytecodes = symbol->data;
                for (int i = 0; i < bytecodes->count; i++) {
                    ctr_bytecode_t* bytecode = list_get(bytecodes, i);
                    printf("\t%s\n", asm_opcode2mnemonic(bytecode->instruction));
                }
                break;
            }
            case CTR_SYMBOL_INT: {
               printf("\t%i\n", *(int32_t*)symbol->data);
               break;
            }
            case CTR_SYMBOL_STR: {
                int32_t* string = symbol->data;
                printf("\t'");
                for (int i = 0; i < string[0]; i++) {
                    printf("%c", (char)string[i+1]);
                }
                printf("'\n");
                break;
            }
        }
    }
    asm_release();

    // write
    //ctr_write(output, &container);
    fclose(output);

    return 0;
}

static ctr_t read(FILE* file) {
    ctr_t container;
    ctr_init(&container);
    void* symbols = &container.nsymbols;

    int segment = SEG_DATA;

    while(!feof(file)) {
        char line[BUFFER_SIZE];
        fgets(line, BUFFER_SIZE, file);
        line[strlen(line) - 1] = '\0';

        char first = line[0];
        ctr_symbol_t symbol;

        switch (first) {
            case '#': break;
            case '.': 
                if (strcmp(line, ".data") == 0) { segment = SEG_DATA; break; }
                if (strcmp(line, ".text") == 0) { segment = SEG_TEXT; break; }
                fprintf(stderr, "unkown segment '%s'\n", line); break;
            case '@': {
                if (segment != SEG_TEXT) { fprintf(stderr, "bad line '%s'", line); continue; }
                symbol.type = CTR_SYMBOL_FUNC;
                symbol.data = malloc(sizeof(list_t));
                list_init(symbol.data, sizeof(ctr_bytecode_t));
                map_set(symbols, func_name(line), &symbol);
                break;
            }
            default:
                switch (segment) {
                    case SEG_DATA: {
                        char* type = strtok(line, " \t\r\b");
                        if (!type) break;
                        char* name = strtok(NULL, " \t\r\b");
                        char* value = strtok(NULL, " \t\r\b");

                        ctr_symbol_t symbol = assemble_data(type, value);
                        map_set(symbols, name, &symbol);
                        break;
                    }
                    case SEG_TEXT: {
                        char* mnemonic = strtok(line, " \t\r\n");
                        if (!mnemonic) break;
                        char* arg1 = strtok(NULL, " \t\r\n");
                        char* arg2 = strtok(NULL, " \t\r\n");
                        ctr_bytecode_t bytecode = assemble(mnemonic, arg1, arg2, symbols);
                        list_add(symbol.data, &bytecode);
                        break;
                    }
                }
        }
    }

    return container;
}

static char* func_name(char* line) {
    return &line[1];
}

static ctr_bytecode_t assemble(char* mnemonic, char* arg1, char* arg2, void* symbols) {
    ctr_bytecode_t bytecode;
    bytecode.instruction = asm_mnemonic2opcode(mnemonic);
    bytecode.argument = 0x0;

    switch (bytecode.instruction) {
        case ASM_CALLE: {
            ctr_symbol_t symbol;
            symbol.type = CTR_SYMBOL_EXTERN;
            symbol.data = NULL;
            map_set(symbols, arg1, &symbol);
            break;
        }
    }

    return bytecode;
}

static ctr_symbol_t assemble_data(char* type, char* value) {
    ctr_symbol_t symbol;
    if (strcmp(type, "integer") == 0) {
        symbol.type = CTR_SYMBOL_INT;
        symbol.data = malloc(sizeof(int32_t));
        *(int32_t*)symbol.data = atoi(strtok(value, " \t\r\n"));
        return symbol;
    }

    if (strcmp(type, "string") == 0) {
        symbol.type = CTR_SYMBOL_STR;
        char* str = strtok(value, " \t\r\n\"");
        int32_t length = strlen(str);
        symbol.data = calloc(sizeof(int32_t), length + 1);
        int32_t* string = symbol.data;
        string[0] = length;
        for (int i = 0; i < length; i++) {
            string[i + 1] = (int32_t)str[i];
        }
        return symbol;
    }
    return symbol;
}
