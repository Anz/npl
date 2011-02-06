#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

/**
 * 4 Bytes pointer
 * 4 Bytes size
 * 4 Bytes element size
 */

void array_init(void* array, int32_t data_size);
void array_release(void* array);
void array_add(void* array, void* element);

#endif
