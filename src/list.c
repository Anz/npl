#include "list.h"
#include <stdlib.h>
#include <string.h>

void list_set_pointer(list_node node, void* pointer) {
    memcpy(node, &pointer, sizeof(void*));
}

void list_set_node(list_node node, void* data, size_t size) {
    list_set_pointer(node, NULL);
    memcpy(node + sizeof(void*), data, size);
}

void* list_get_next(list_node node) {
    void* pointer;
    memcpy(&pointer, node, sizeof(void*));
    return pointer;
}

void list_init(list_t* list, size_t data_size) {
    list->count = 0;
    list->node_size = sizeof(void*) + data_size;
    list->first = NULL;
    list->last = NULL;
}

void list_add(list_t* list, void* data) {
    void* node = calloc(list->node_size, sizeof(char));
    list_set_node(node, data, list->node_size - sizeof(void*));

    // if list is empty
    if (list->count == 0) {
        list->first = node;
    } else {
        list_set_pointer(list->last, node);
    }

    list->last = node;
    list->count++;
}

void* list_data(list_node node) {
    return node + sizeof(void*);
}

list_node list_get(list_t* list, unsigned int index) {
    list_node node = list_first(list);
    unsigned int count = 0;
    while (node != NULL) {
        if (count == index) {
            return node;
        }
        node = list_next(node);
        count++;
    }
    return NULL;
}

int list_find(list_t* list, void* data) {
    list_node node = list_first(list);
    int index = 0;
    while (node != NULL) {
        if (memcmp(list_data(node), data, list->node_size) == 0) {
            return index;
        }
        node = list_next(node);
        index++;
    }
    return -1;

}

list_node list_first(list_t* list) {
    return list->first;
}

list_node list_next(list_node iterator) {
    return list_get_next(iterator);
}

void list_release(list_t* list) {
    list_node previous = NULL;
    list_node current = list_first(list);

    while (previous != NULL || current != NULL) {
        if (previous != NULL) {
            free(previous);
        }

        previous = current;

        if (current != NULL) {
            current = list_next(current);
        }
    }
}
