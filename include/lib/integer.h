#ifndef INTEGER_H
#define INTEGER_H

#define INTEGER_SIZE 4

#include "map.h"

void library_add_integer(map_t* map);
void integer(int* i, int value);
void integer_add(int* result, int* a, int* b);
void integer_sub(int* result, int* a, int* b);
void integer_mul(int* result, int* a, int* b);
void integer_div(int* result, int* a, int* b);

#endif
