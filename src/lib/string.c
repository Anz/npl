#include "lib/string.h"
#include <stdarg.h>

void string_init(lib_string string, int num, ...) {
    array_init(string, 4);

    va_list arguments;

    va_start(arguments, num);
    for (int i = 0; i < num; i++) {
        int value = va_arg(arguments, int);
        array_add(string, &value);
    }
    va_end(arguments);
}
