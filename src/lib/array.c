#include "lib/array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void array_init(void* array, int32_t data_size) {
    printf("list init (%p)  with element size of %i\n", array, data_size);
    int32_t* data = array;
    /**data = 0;
    /*data[0] = 0;
    /*data[1] = 0;
    data[2] = data_size;*/
}

void array_release(void* array) {
    int32_t* data = array;
    printf("delete list with %i number of elements\n", data[2]);
    free((void*)data[0]);
    memset(array, 0, 12);
}

void array_add(void* array, void* element) {
    int32_t* data = array;
    printf("add element to list (%i number of elements)\n", data[2] + 1);
    data[1]++;
    data[0] = (int32_t)realloc(&data[0], data[1] * data[2]);
    void* ptr = (void*)(data[0] + (data[1] - 1) * data[2]);
    memcpy(ptr, element, data[2]);
}
