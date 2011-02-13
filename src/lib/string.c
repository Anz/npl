#include "lib/string.h"
#include "lib/array.h"
#include <stdio.h>
#include <stdarg.h>

void string_init(void* array, int num, ...) {
    va_list arguments;

    va_start(arguments, num);
    for (int i = 0; i < num; i++) {
        int value = va_arg(arguments, int);
        array_add(array, &value);
    }
    va_end(arguments);
}
