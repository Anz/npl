#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

unsigned int swap_endian(unsigned int value);
int isletter(char c);
size_t min_s(size_t a, size_t b);
unsigned int line_index(unsigned int line, size_t line_size, size_t offset);
char* resize_memory(char* buffer, size_t size);
unsigned int char2int(char value);

#endif // UTIL_H
