#include "integer.h"
#include "stdio.h"

void library_add_integer(map_t* map) {
    int func_integer = (int)&integer;
    map_set(map, "integer", &func_integer);
    int func_integer_add = (int)&integer_add;
    map_set(map, "integer_add", &func_integer_add);
    int func_integer_sub = (int)&integer_sub;
    map_set(map, "integer_sub", &func_integer_sub);
    int func_integer_mul = (int)&integer_mul;
    map_set(map, "integer_mul", &func_integer_mul);
    int func_integer_div = (int)&integer_div;
    map_set(map, "integer_div", &func_integer_div);
}

void integer(int* i, int value) {
    printf("integer set %08X from %i to ", (int)i, *i);
    *i = value;
    printf("%i\n", *i);
}

void integer_add(int* result, int* a, int* b) {
    printf("integer add %i + %i = ", *a, *b);
    *result = *a + *b;
    printf("%i\n", *result);
}

void integer_sub(int* result, int* a, int* b) {
    printf("integer sub %i - %i = ", *a, *b);
    *result = *a - *b;
    printf("%i\n", *result);
}

void integer_mul(int* result, int* a, int* b) {
    printf("integer mul %i * %i = ", *a, *b);
    *result = *a * *b;
    printf("%i\n", *result);
}

void integer_div(int* result, int* a, int* b) {
    printf("integer div %i / %i = ", *a, *b);
    *result = *a / *b;
    printf("%i\n", *result);
}

