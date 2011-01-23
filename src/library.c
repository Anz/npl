#include "library.h"
#include "integer.h"

void library_init(library_t* library) {
    map_init(library, MAP_STR, sizeof(int));

    // integer functions
    library_add(library, "integer", &integer);
    library_add(library, "integer_add", &integer_add);
    library_add(library, "integer_sub", &integer_sub);
    library_add(library, "integer_mul", &integer_mul);
    library_add(library, "integer_div", &integer_div);
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
