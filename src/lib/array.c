#include "lib/array.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int32_t* array_memory(lib_array array) {
    return (int32_t*)*(int32_t*)array;
}

static void set_array(lib_array array, int32_t* memory) {
    *(int32_t*)array = (int32_t)memory;
}

static int32_t get_count(lib_array array) {
    return array_memory(array)[0];
}

static int32_t get_element_size(lib_array array) {
    return array_memory(array)[1];
}

static void print_array(lib_array array, char* text) {
    int32_t* memory = array_memory(array);
    //printf("array %s (%p): (%p) with %i elements by a size of %i\n", text, array, memory, memory[0], memory[1]);
}

void array_init(lib_array array, int32_t data_size) {
    int32_t* memory = calloc(sizeof(int32_t), 2);
    memory[0] = 0;
    memory[1] = data_size;
    set_array(array, memory);
    print_array(array, "init");
}

void array_release(lib_array array) {
    print_array(array, "release");
    int32_t* memory = array_memory(array);
    free(memory);
    set_array(array, NULL);
}

void array_add(lib_array array, void* element) {
    int32_t* memory = array_memory(array);
    memory[0]++;
    memory = realloc(memory, (memory[0] * memory[1] + 2 * sizeof(int32_t)));
    set_array(array, memory);
    void* ptr = ((void*)&memory[2]) + (memory[0] - 1) * memory[1];
    memcpy(ptr, element, memory[1]);
    print_array(array, "add");
}
