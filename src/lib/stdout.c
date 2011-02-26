#include "lib/stdout.h"
#include "lib/string.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void print(int num, ...) {
    va_list arguments;
    va_start(arguments, num);

    size_t size = 1;
    char* buffer = malloc(size);

    for (int i = 0; i < num; i++) {
        lib_string string = va_arg(arguments, lib_string);
        int32_t* memory = array_memory(string);
        size_t length = memory[0];
        size += length;
        buffer = realloc(buffer, size);
        for (int i = 0; i < length; i++) {
            buffer[size - length - 1 + i] = (char)memory[i + 2];
        }
    }

    buffer[size - 1] = '\0';
    va_end(arguments);
    printf("print: %s\n", buffer);
    free(buffer);
}
