#include "integer.h"
#include "stdio.h"

void library_add_integer(map_t* map) {
    int func_integer = (int)&integer;
    map_add(map, "integer", &func_integer);
    //map_add(map, "integer_set", integer_set);
}

void integer(int* i) {
    printf("integer %08X\n", (int)i);
    *i = 0;
    printf("integer %i\n", *i);
}

void integer_set(int* i) {
    *i = 1337;
}
