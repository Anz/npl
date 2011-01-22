#include "util.h"
#include <stdlib.h>
#include <string.h>

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

unsigned int char2int(char value) {
    unsigned int new_value = value;
    return (new_value << 24) >> 24; 
}

char int2char(int value, unsigned int index) {
    value = swap_endian(value);
    char* ptr = (char*)&value;
    return ptr[index];
}

char number2str(int value) {
    return 'a' + value;
}

size_t strnlen(char* str, size_t maxlen) {
    char* end = memchr(str, '\0', maxlen);
    if (end) {
        return (size_t)(end - str);
    } 
    
    return maxlen;
}
