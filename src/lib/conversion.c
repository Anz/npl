#include "lib/conversion.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

void decimal_to_string(lib_string string, lib_integer* decimal) {
    size_t length = (size_t)log10((double)*decimal) + 1;
    char buffer[length + 1];
    sprintf(buffer, "%d", *decimal);

    string_init(string, 0);
    for (size_t s = 0; s < length; s++) {
        lib_integer c = (lib_integer)buffer[s];
        array_add(string, &c);
    }
    printf("d2s: %s <%i>\n", buffer, length);
}
