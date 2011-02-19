#ifndef INTEGER_H
#define INTEGER_H

#define INTEGER_SIZE 4

#include <stdint.h>
#include "map.h"

typedef int32_t lib_integer;

void library_add_integer(map_t* map);
void integer(lib_integer* i, lib_integer value);
void integer_add(lib_integer* result, lib_integer* a, lib_integer* b);
void integer_sub(lib_integer* result, lib_integer* a, lib_integer* b);
void integer_mul(lib_integer* result, lib_integer* a, lib_integer* b);
void integer_div(lib_integer* result, lib_integer* a, lib_integer* b);

#endif
