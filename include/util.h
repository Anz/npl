#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

unsigned int swap_endian(unsigned int value);
int isletter(char c);
size_t min_s(size_t a, size_t b);
unsigned int char2int(char value);
char int2char(int value, unsigned int index);
char number2str(int value);
size_t strnlen(char* str, size_t maxlen);

#endif // UTIL_H
