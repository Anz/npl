#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

typedef void* lib_array;

int32_t* array_memory(lib_array array);
void array_init(lib_array array, int32_t data_size);
void array_release(lib_array array);
void array_add(lib_array array, void* element);

#endif
