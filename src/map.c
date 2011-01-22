#include "map.h"
#include <stdlib.h>
#include <string.h>

int compare(void* a, void* b, size_t size) {
    if (size == MAP_STR) {
        // string mode (variable length)
        return strcmp(a, b) == 0;
    } else  {
        // fixed binary size mode
        return memcmp(a, b, size) == 0;
    }
}

void copy(void** a, void* b, size_t size) {
    if (size == MAP_STR) {
        // string mode (variable length)
        size_t len = strlen(b) + 1;
        *a = realloc(*a, len);
        memcpy(*a, b, len);
    } else  {
        // fixed binary size mode
        memcpy(*a, b, size);
    }
}

void* memory(size_t size, void* data) {
    if (size == MAP_STR) {
        size_t len = strlen(data) + 1;
        return malloc(len);
    } else {
        return malloc(size);
    }
}

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
        free(entry->value);
    }
}

// find entry by key and return value
void* map_find_key(map_t* map, void* key) {
     for (unsigned int i = 0; i < map->list.count; i++) {
        map_entry_t* entry = (map_entry_t*)list_get(&map->list, i);
        if (compare(entry->key, key, map->key_size)) {
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
        if (compare(entry->value, value, map->value_size)) {
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
        if (compare(entry->key, key, map->key_size)) {
            copy(&entry->value, value, map->value_size);
            return;
        }
    }

    // key does not exists yet
    map_entry_t entry;
    entry.key = memory(map->key_size, key);
    entry.value = memory(map->value_size, value);
    copy(&entry.key, key, map->key_size);
    copy(&entry.value, value, map->value_size);
    list_add(&map->list, &entry);
}

