#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

typedef void* lib_array;

/**
 * 4 Bytes pointer
 * 4 Bytes size
 * 4 Bytes element size
 */

void array_init(lib_array array, int32_t data_size);
void array_release(lib_array array);
void array_add(lib_array array, void* element);

#endif
