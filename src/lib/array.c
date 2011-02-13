#include "lib/array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void print_array(void* array, char* text) {
/*    int32_t* data = array;
    printf("array %s (%p): (%p) with %i elements by a size of %i\n", text, array, (void*)data[0], data[1], data[2]);
    int32_t* values = (int32_t*)data[0];*/
}


void array_init(void* array, int32_t data_size) {
    int32_t* data = array;
    data[0] = 0;
    data[1] = 0;
    data[2] = data_size;
    print_array(array, "init");
}

void array_release(void* array) {
    print_array(array, "release");
    int32_t* data = array;
    free((void*)data[0]);
    memset(array, 0, 12);
}

void array_add(void* array, void* element) {
    int32_t* data = array;
    data[1]++;
    data[0] = (int32_t)realloc((void*)data[0], data[1] * data[2]);
    void* ptr = (void*)(data[0] + (data[1] - 1) * data[2]);
    //void* ptr = (void*)data[0];
    //ptr += (data[1] - 1) * data[2];
    memcpy(ptr, element, data[2]);
    //memcpy(ptr, ptr, data[2]);
    print_array(array, "add");
}
