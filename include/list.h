#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef void* list_node;

typedef struct list {
    unsigned int count;
    size_t node_size;
    list_node first;
    list_node last;
} list_t;

void list_init(list_t* list, size_t data_size);
void list_add(list_t* list, void* data);
void* list_data(list_node node);
list_node list_get(list_t* list, unsigned int index);
int list_find(list_t* list, void* data);
list_node list_first(list_t* list);
list_node list_next(list_node iterator);
void list_release(list_t* list);

#endif // LIST_H
