#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// node init
void node_init(map_node_t* node, char* key, void* value, size_t size, map_node_t* parent) {
    int length = strlen(key) + 1;
    node->key = malloc(length);
    memcpy(node->key, key, length);

    node->value = malloc(size);
    memcpy(node->value, value, size);

    node->parent = parent;
    node->less = NULL;
    node->greater = NULL;
}

// iterate through all nodes and execute function
void node_iterate(map_node_t* node, void (*func)(map_node_t* node)) {
    if (node == NULL) {
        return;
    }
    node_iterate(node->less, func);
    node_iterate(node->greater, func);
    func(node);
}

// map init
void map_init(map_t* map, size_t node_size) {
    map->node_size = node_size;
    map->root = NULL;
}

// map realase
void map_delete_all(map_node_t* node) {
    if (node == NULL) {
        return;
    }
    free(node->key);
    free(node->value);
    free(node);
}

void map_release(map_t* map) {
    node_iterate(map->root, map_delete_all);
    map->root = NULL;
}

// find
map_node_t* node_find(map_node_t* node, char* key) {
    if (node == NULL) { // not found
        return NULL;
    }

    int compare = strcmp(node->key, key);
    if (compare == 0) { // equal
        return node;
    } else if (compare < 0) { // less
        return node_find(node->less, key);
    } else { // greater
        return node_find(node->greater, key);
    }
}

map_node_t* map_find(map_t* map, char* key) {
   return node_find(map->root, key);
}

map_node_t* map_findi(map_t* map, int key) {
    char ckey[9];
    sprintf(ckey, "%x", key);
    return map_find(map, ckey);
}

// add
void node_add(map_node_t* node, char* key, void* value, size_t size) {
    int compare = strcmp(node->key, key);
    if (compare == 0) { // equal
        int length = strlen(key) + 1;
        node->key = realloc(node->key, length);
        memcpy(node->key, key, length);
        memcpy(node->value, value, size);
    } else if (compare < 0) { // less
        if (node->less == NULL) {
            node->less = malloc(sizeof(map_node_t));
            node_init(node->less, key, value, size, node);
        } else {
            node_add(node->less, key, value, size);
        }
    } else { // greater
        if (node->greater == NULL) {
            node->greater = malloc(sizeof(map_node_t));
            node_init(node->greater, key, value, size, node);
        } else {
            node_add(node->greater, key, value, size);
        }
    }

}
void map_add(map_t* map, char* key, void* value) {
    if (map->root == NULL) {
        map->root = malloc(sizeof(map_node_t));
        node_init(map->root, key, value, map->node_size, NULL);
    } else {
        node_add(map->root, key, value, map->node_size);
    }
}

void map_addi(map_t* map, int key, void* value) {
    char ckey[9];
    sprintf(ckey, "%x", key);
    map_add(map, ckey, value);
}

// print tree
void print_node(map_node_t* node) {
    printf("key:");
    map_node_t* tmp = node;
    while (tmp != NULL) {
        tmp = tmp->parent;
        printf(" ");
    }

    printf("%s\n", node->key);
}

void map_print_tree(map_t* map) {
    printf("### tree start ###\n");
    node_iterate(map->root, print_node);
    printf("### tree end ###\n\n");
}
