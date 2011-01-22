#include "map.h"
#include <stdlib.h>
#include <string.h>

// init map
void map_init(map_t* map, size_t key_size, size_t value_size) {
    map->key_size = key_size;
    map->value_size = value_size;
    list_init(&map->list, sizeof(map_entry_t));
}

// release map
void map_release(map_t* map) {
    for (int i = 0; i < map->list.count; i++) {
        map_entry_t* entry = (map_entry_t*)list_get(&map->list, i);
        free(entry->key);
    }
}

// find entry by key and return value
void* map_find_key(map_t* map, void* key) {
     for (unsigned int i = 0; i < map->list.count; i++) {
        map_entry_t* entry = (map_entry_t*)list_get(&map->list, i);
        if (memcmp(entry->key, key, map->key_size) == 0) {
            return entry->value;
        }
    }
    
    // not found
    return NULL;
}

// find entry by value and return key
void* map_find_value(map_t* map, void* value) {
   for (unsigned int i = 0; i < map->list.count; i++) {
        map_entry_t* entry = (map_entry_t*)list_get(&map->list, i);
        if (memcmp(entry->value, value, map->value_size) == 0) {
            return entry->key;
        }
    }
    
    // not found
    return NULL;

}

// set or add value of a giben key
void map_set(map_t* map, void* key, void* value) {
    // key already exists
    for (unsigned int i = 0; i < map->list.count; i++) {
        map_entry_t* entry = (map_entry_t*)list_get(&map->list, i);
        if (memcmp(entry->key, key, map->key_size) == 0) {
            memcpy(entry->value, value, map->value_size);
            return;
        }
    }

    // key does not exists yet
    void* memory = malloc(map->key_size + map->value_size);
    map_entry_t entry;
    entry.key = memory;
    entry.value = memory + map->key_size;
    memcpy(entry.key, key, map->key_size);
    memcpy(entry.value, value, map->value_size);
    list_add(&map->list, &entry);
}

