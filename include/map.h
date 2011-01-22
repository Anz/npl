#ifndef MAP_H
#define MAP_H

#include "list.h"

#define MAP_STR 0

// map entry
typedef struct map_entry {
    void* key;
    void* value;
} map_entry_t;

// map
typedef struct map {
    size_t key_size;
    size_t value_size;
    list_t list;
} map_t;

void map_init(map_t* map, size_t key_size, size_t value_size);
void map_release(map_t* map);
void* map_find_key(map_t* map, void* key);
void* map_find_value(map_t* map, void* value);
void map_set(map_t* map, void* key, void* value);

#endif
