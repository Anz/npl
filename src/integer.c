#include "integer.h"
#include "stdio.h"

void library_add_integer(map_t* map) {
    int func_integer = (int)&integer;
    map_add(map, "integer", &func_integer);
    int func_integer_add = (int)&integer_add;
    map_add(map, "integer_add", &func_integer_add);
    int func_integer_sub = (int)&integer_sub;
    map_add(map, "integer_sub", &func_integer_sub);
    int func_integer_mul = (int)&integer_mul;
    map_add(map, "integer_mul", &func_integer_mul);
    int func_integer_div = (int)&integer_div;
    map_add(map, "integer_div", &func_integer_div);
}

void integer(int* i, int value) {
    printf("integer set %08X from %i to ", (int)i, *i);
    *i = value;
    printf("%i\n", *i);
}

void integer_add(int* i, int* value) {
    printf("integer add %08X from %i to ", (int)i, *i);
    *i += *value;
    printf("%i\n", *i);
}

void integer_sub(int* i, int* value) {
    printf("integer sub %08X from %i to ", (int)i, *i);
    *i -= *value;
    printf("%i\n", *i);
}

void integer_mul(int* i, int* value) {
    printf("integer mul %08X from %i to ", (int)i, *i);
    *i *= *value;
    printf("%i\n", *i);
}

void integer_div(int* i, int* value) {
    printf("integer_set %08X from %i to ", (int)i, *i);
    *i /= *value;
    printf("%i\n", *i);
}

