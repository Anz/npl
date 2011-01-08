#ifndef INTEGER_H
#define INTEGER_H

#define INTEGER_SIZE 4

#include "map.h"

void library_add_integer(map_t* map);
void integer(int* i, int value);
void integer_add(int* i, int* value);
void integer_sub(int* i, int* value);
void integer_mul(int* i, int* value);
void integer_div(int* i, int* value);

#endif
