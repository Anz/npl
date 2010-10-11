#include "assembler.h"
#include <string.h>

#define ASM_SEGMENT_NONE 0x00
#define ASM_SEGMENT_TEXT 0x01
#define ASM_SEGMENT_DATA 0x02

void assembler_read_line(char* line, size_t size);
int min(int a, int b);

int assembler_next_segment();
int assembler_text_next_functon(nof_t* nof);

int assembly_index = 0;
int assembly_line = 0;
int assembly_text_line = 0;
char*  assembly_content = 0;
size_t assembly_size = 0;

void assembler(nof_t* nof, char* assembly, size_t size) {

    assembly_content = assembly;
    assembly_size = size;

    int segment = assembler_next_segment();
    while (segment != -1) {
        switch (segment) {
            case ASM_SEGMENT_TEXT: {
                printf("text segment\n");
                while(assembler_text_next_functon(nof) != 0);
                break;
            }
            case ASM_SEGMENT_DATA: {
                printf("data segment\n");
                break;
            }
            default: {
                printf("wrong segment\n");
            }
        }

        segment = assembler_next_segment();
    }
}

int assembler_next_segment() {
    char line[255];
    assembler_read_line(line, 255);
    while(assembly_index < assembly_size) {
        if (strlen(line) > 0 && line[0] == '.') {
            if (memcmp("data", &line[1], 4) == 0) {
                return ASM_SEGMENT_DATA;
            } else if (memcmp("text", &line[1], 4) == 0) {
                assembly_text_line = assembly_line;
                return ASM_SEGMENT_TEXT;
            } else {
                return ASM_SEGMENT_NONE;
            }
        }

        assembler_read_line(line, 255);
    }
    return -1;
}

int assembler_text_next_functon(nof_t* nof) {
    char line[255];
    assembler_read_line(line, 255);
    while(assembly_index < assembly_size) {
        if (strlen(line) > 0 && line[0] != ' ' && line[0] != '\t') {
            nof_symbol_add(nof, nof->text + assembly_line - assembly_text_line, line);
            return 1;
        }

        assembler_read_line(line, 255);
    }
    return 0;
}

void assembler_read_line(char* line, size_t size) {
    if (assembly_index >= assembly_size) {
        return;
    }

    int length;
    for (length = 0; assembly_index+length < assembly_size && assembly_content[assembly_index+length] != '\n'; length++);
    memset(line, 0, size);
    memcpy(line, &assembly_content[assembly_index], min(length, size));
    line[size-1] = '\0';
    assembly_index += length + 1;
    assembly_line++;
}

int min(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}
