#include "lib/library.h"
#include "lib/integer.h"
#include "lib/array.h"
#include "lib/stdout.h"
#include "lib/string.h"
#include "lib/conversion.h"

void library_init(library_t* library) {
    map_init(library, MAP_STR, sizeof(int));

    // integer functions
    library_add(library, "integer", &integer);
    library_add(library, "integer_add", &integer_add);
    library_add(library, "integer_sub", &integer_sub);
    library_add(library, "integer_mul", &integer_mul);
    library_add(library, "integer_div", &integer_div);

    // array functions
    library_add(library, "list_init", &array_init);
    library_add(library, "list_release", &array_release);
    library_add(library, "list_add", &array_add);

    // stdout functions
    library_add(library, "print", &print);

    // string functions
    library_add(library, "string_init", &string_init);
    library_add(library, "string_release", &array_release);

    // conversion functions
    library_add(library, "decimal_to_string", &decimal_to_string);
}

void library_release(library_t* library) {
    map_release(library);
}

void library_add(library_t* library, char* symbol, void* func) {
    map_set(library, symbol, &func);
}

void* library_get(library_t* library, char* symbol) {
    int* value = map_find_key(library, symbol);
    if (!value) {
        return NULL;
    }
    return (void*)*value;
}
