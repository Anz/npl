#include "util.h"
#include <stdlib.h>

unsigned int swap_endian(unsigned int value) {
    return (value << 24) |
           ((value << 8) & 0x00FF0000) |
           ((value >> 8) & 0x0000FF00) |
           (value >> 24);
}

int isletter(char c) {
    return 'a' <= c && c <= 'z';
}

size_t min_s(size_t a, size_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

unsigned int line_index(unsigned int line, size_t line_size, size_t offset) {
    return line * line_size + offset;
}

char* resize_memory(char* buffer, size_t size) {
    char* old = buffer;
    buffer = realloc(buffer, size);

    if (buffer == NULL) {
        free(old);
    }
    return buffer;
}

unsigned int char2int(char value) {
    unsigned int new_value = value;
    return (new_value << 24) >> 24; 
}

char int2char(int value, unsigned int index) {
    value = swap_endian(value);
    char* ptr = &value;
    return ptr[index];
}
