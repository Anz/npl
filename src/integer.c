#include "integer.h"
#include "stdio.h"

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

