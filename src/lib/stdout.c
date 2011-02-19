#include "lib/stdout.h"
#include "lib/string.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void print(int num, ...) {
    va_list arguments;
    va_start(arguments, num);

    size_t size = 1;
    char* buffer = NULL;

    for (int i = 0; i < num; i++) {
        lib_string string = va_arg(arguments, lib_string);
        int32_t* data = string;
        int32_t* chars = (int32_t*)data[0];
        size_t length = data[1];
        size += length;
        buffer = realloc(buffer, size);
        for (int i = 0; i < length; i++) {
            buffer[size - length - 1 + i] = (char)chars[i];
        }
    }

    buffer[size - 1] = '\0';
    va_end(arguments);
    printf("print: %s\n", buffer);
}
