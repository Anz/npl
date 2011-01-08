#include "integer.h"
#include "stdio.h"

void library_add_integer(map_t* map) {
    int func_integer = (int)&integer;
    map_add(map, "integer", &func_integer);
    int func_integer_set = (int)&integer_set;
    map_add(map, "integer_set", &func_integer_set);
}

void integer(int* i) {
    printf("integer %08X from %i to ", (int)i, *i);
    *i = 0;
    printf("%i\n", *i);
}

void integer_set(int* i, int value) {
    printf("integer_set %08X from %i to ", (int)i, *i);
    *i = value;
    printf("%i\n", *i);
}
