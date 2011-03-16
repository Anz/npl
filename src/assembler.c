#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assembly.h"
#include "util.h"
#include "container.h"
#include <stdint.h>

#define READ_BUFFER_SIZE 1024
#define SEGMENT_BUFFER_SIZE 1024
#define ASM_DATA_SEGMENT "data"
#define ASM_TEXT_SEGMENT "text"
#define ASM_COMMENT '#'
#define ASM_SEGMENT '.'
#define ASM_TAB '\t'
#define ASM_SPACE ' '

typedef struct command {
    char code;
    list_t args;
} command_t;

void read_text_segment(FILE* input, ctr_t* container, list_t* commands);

void read_data_segment(FILE* input, ctr_t* container, list_t* commands) {
    char line[READ_BUFFER_SIZE];

    while(!feof(input)) {
        fgets(line, READ_BUFFER_SIZE, input);

        switch(line[0]) {
            // comment
            case '#': continue;

            // segment change
            case '.': {
                char* segment = strtok(line, ". \t\r\n");
                if (strcmp(segment, "text") == 0) {
                    read_text_segment(input, container, commands);
                    return;
                } else if (strcmp(segment, "data") != 0) {
                    fprintf(stderr, "error: unknown segment %s\n", segment);
                    return;
                }
                continue;
            }
            // data
            default: {
                ctr_symbol_t symbol;
                char* type = strtok(line, " \t\r\n");
                char* name = strtok(NULL, " \t\r\n");
                char* data = NULL;
                
                if (type) {
                    if (strcmp(type, "string") == 0) {
                        strtok(NULL, "\"\r\n");
                        data = strtok(NULL, "\"\t\r\n");
                        int32_t length = strlen(data);
                        int32_t type_size = length * sizeof(int32_t);
                        symbol.type = CTR_SYMBOL_STR;
                        symbol.data = malloc(type_size + sizeof(int32_t));

                        container->header.data_size += type_size;
                        int32_t* nstring = symbol.data;
                        nstring[0] = length;
                        for (int32_t i = 0; i < length; i++) {
                            nstring[i + 1] = (int32_t)data[i];
                        }
                    } else if(strcmp(type, "integer") == 0) {
                        data = strtok(NULL, " \t\r\n");
                        int32_t value = swap_endian(atoi(data));
                        symbol.type = CTR_SYMBOL_INT;
                        symbol.data = malloc(sizeof(int32_t));
                        *(int32_t*)symbol.data = swap_endian(value);
                        container->header.data_size += sizeof(int32_t);
                    }
                    printf("'%s' of type '%s' with data = '%s'\n", name, type, data);
                    map_set(&container->nsymbols, name, &symbol);
                }
            }
        }
    }
}

void read_text_segment(FILE* input, ctr_t* container, list_t* commands) {
    fseek(input, 0, SEEK_CUR);
    map_t* symbols = &container->symbols;
    map_t* externals = &container->externals;

    ctr_addr addr = 0;
    char line[READ_BUFFER_SIZE];
    int external_index = 0;

    while(!feof(input)) {
        fgets(line, READ_BUFFER_SIZE, input);

        switch(line[0]) {
            // comment
            case '#': continue;

            // segment change
            case '.': {
                char* segment = strtok(line, ". \t\r\n");
                if (strcmp(segment, "data") == 0) {
                    read_data_segment(input, container, commands);
                    return;
                } else if (strcmp(segment, "text") != 0) {
                    fprintf(stderr, "error: unknown segment %s\n", segment);
                    return;
                }
            }
        }

        char* colon = strchr(line, ':');
        char* instruction = NULL;
        if (colon == NULL) {
            instruction = strtok(line, " \t\r\n");
        } else {
            instruction = strtok(colon+1, " \t\r\n"); 
        }

        if (colon != NULL) {
            char* name = strtok(line, ": ");
            // add symbol to map
            map_set(symbols, name, &addr);

            ctr_symbol_t symbol;
            symbol.data = malloc(sizeof(list_t));
            list_init(symbol.data, sizeof(ctr_bytecode_t));
            symbol.type = CTR_SYMBOL_FUNC;
            map_set(&container->nsymbols, name, &symbol);
        }

        if (instruction) {
            command_t command;
            command.code = asm_mnemonic2opcode(instruction);
            list_init(&command.args, 50);
            char* arg = strtok(NULL, " ,\t\r\n");
            while (arg) {
                char text[50];
                memset(text, 0, 50);
                strncpy(text, arg, 50);
                list_add(&command.args, text);
                arg = strtok(NULL, " ,\t\r\n");
            }
            if (command.code == ASM_CALLE) {
                char* key = (char*)list_get(&command.args, 0);
                if (!map_find_key(externals, key)) {
                    map_set(externals, key, &external_index);
                    external_index++;
                }

                ctr_symbol_t symbol;
                symbol.data = NULL;
                symbol.type = CTR_SYMBOL_EXTERN;
                map_set(&container->nsymbols, key, &symbol);
            }
            list_add(commands, &command);
            addr++;
        }
    }
}

void assembler(FILE* input, FILE* output) {
    // init assembly table
    asm_init();

    ctr_t container;
    ctr_init(&container);
    map_t* symbols = &container.symbols;
    map_t* externals = &container.externals;
    list_t* texts = &container.texts;

    list_t commands;
    list_init(&commands, sizeof(command_t));

    // read file
    read_data_segment(input, &container, &commands);

    // variables
    map_t variables;
    map_init(&variables, MAP_STR, sizeof(int));

    // types
    map_t types;
    map_init(&types, MAP_STR, sizeof(int));
    int integer = 4;
    int list = 4;
    map_set(&types, "integer", &integer);
    map_set(&types, "list", &list);
    map_set(&types, "string", &list);

    for (unsigned int i = 0; i < commands.count; i++) {
        command_t* command = list_get(&commands, i);
        ctr_bytecode_t bc;
        bc.instruction = command->code;
        bc.argument = 0;

        char* arg1 = list_get(&command->args, 0);
        if (command->args.count > 0) {
            switch (command->code) {
                case ASM_JMP:
                case ASM_JE:
                case ASM_JNE:
                case ASM_JL:
                case ASM_JLE:
                case ASM_JG:
                case ASM_JGE:
                case ASM_CALL: {
                    ctr_addr* addr = (int*)map_find_key(symbols, arg1);
                    if (addr) {
                        // maybe **addr
                        bc.argument = *addr - i;
                    } else {
                        fprintf(stderr, "sync address not found %s\n", arg1); 
                    }
                    break;
                }
                case ASM_CALLE: { 
                    int* index = map_find_key(externals, arg1);
                    if (index) {
                        bc.argument = *index;
                    } else {
                        fprintf(stderr, "(a)synce address not found %s\n", arg1); 
                    }
                    break;
                }
                case ASM_ENTER: {
                    for (unsigned int i = 0; i + 1 < command->args.count; i += 2) {
                        char* type = list_get(&command->args, i);
                        char* name = list_get(&command->args, i+1);
                        int* type_size = map_find_key(&types, type);
                        if (!type_size) {
                            fprintf(stderr, "error could not find type: %s\n", type);
                            continue;
                        }
                        bc.argument += *type_size;
                        map_set(&variables, name, &bc.argument);
                    }
                    break;
                }
                case ASM_ARG: {
                    ctr_addr* variable = map_find_key(&variables, arg1);
                    if (variable) {
                        bc.argument = *variable;
                    } else {
                        fprintf(stderr, "error could not find variable: %s\n", arg1);
                    }
                    break;
                }
                case ASM_ARGV: {
                    switch (arg1[0]) {
                        case '\'': 
                            bc.argument = arg1[1];
                            break;
                        case '"': {
                            int length;
                            for (length = 0; arg1[length+1] != '"'; length++);
                            length--;
                            for (int i = length; i > 0; i--) {
                                ctr_bytecode_t argv;
                                argv.instruction = ASM_ARGV;
                                argv.argument = arg1[i];
                                list_add(texts, &argv);
                            }
                            bc.argument = length;

                            break;
                        }
                        default: 
                            bc.argument = atoi(arg1);
                            break;
                    }
                    break;
                }
            }
        }

        list_add(texts, &bc);
    }

    // write container
    ctr_write(output, &container);

    // release
    ctr_release(&container);
    map_release(&variables);
    asm_release();
    map_release(&types);

}

void print_usage();

int main(int argc, char* argv[]) {
    // usage
    if (argc < 3) {
        print_usage();
        return 0;
    }

    // input
    char* input_path = argv[1];
    FILE* input = fopen(input_path, "r");
    if (input == NULL) {
        fprintf(stderr, "can not open file: %s\n", input_path);
        return 1;
    }

    // ouput
    char* output_path = argv[2];
    FILE* output = fopen(output_path, "wb");
    if (output == NULL) {
        fprintf(stderr, "can not open file: %s\n", output_path);
        return 1;
    }

    // assembler
    assembler(input, output);

    // close file
    fclose(input);
    fclose(output);

    return 0;
}

void print_usage() {
    printf("usage: nas <input> <output>\n");
}
