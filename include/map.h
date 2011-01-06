#ifndef MAP_H
#define MAP_H

#include <stddef.h>

// node
typedef struct map_node {
    char* key;
    void* value;
    struct map_node* parent;
    struct map_node* less;
    struct map_node* greater;
} map_node_t;

// map
typedef struct map {
    size_t node_size;
    map_node_t* root;
} map_t;

void map_init(map_t* map, size_t node_size);
void map_release(map_t* map);
map_node_t* map_find(map_t* map, char* key);
map_node_t* map_findi(map_t* map, int key);
void map_add(map_t* map, char* key, void* value);
void map_addi(map_t* map, int key, void* value);

void map_print_tree(map_t* map);

#endif
