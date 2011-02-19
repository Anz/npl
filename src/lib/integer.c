#include <stdio.h>
#include "lib/integer.h"

void integer(lib_integer* i, lib_integer value) {
    printf("integer set %p from %i to ", i, *i);
    *i = value;
    printf("%i\n", *i);
}

void integer_add(lib_integer* result, lib_integer* a, lib_integer* b) {
    printf("integer add %i + %i = ", *a, *b);
    *result = *a + *b;
    printf("%i\n", *result);
}

void integer_sub(lib_integer* result, lib_integer* a, lib_integer* b) {
    printf("integer sub %i - %i = ", *a, *b);
    *result = *a - *b;
    printf("%i\n", *result);
}

void integer_mul(lib_integer* result, lib_integer* a, lib_integer* b) {
    printf("integer mul %i * %i = ", *a, *b);
    *result = *a * *b;
    printf("%i\n", *result);
}

void integer_div(lib_integer* result, lib_integer* a, lib_integer* b) {
    printf("integer div %i / %i = ", *a, *b);
    *result = *a / *b;
    printf("%i\n", *result);
}

