#include "lib/stdout.h"
#include <stdio.h>
#include <stdint.h>

void print(void* array) {
    int32_t* data = array;
    int32_t* chars = (int32_t*)data[0];
    size_t length = data[1];
    char string[length+1];
    for (int i = 0; i < length; i++) {
        string[i] = (char)chars[i];
    }
    string[length] = '\0';
    printf("print: %s\n", string);
}
