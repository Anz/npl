#ifndef LIBRARY_H
#define LIBRARY_H

#include "map.h"

typedef map_t library_t;

void library_init(library_t* library);
void library_release(library_t* library);
void library_add(library_t* library, char* symbol, void* func);
void* library_get(library_t* library, char* symbol);

#endif
