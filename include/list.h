#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct list_item {
    struct list_item* previous;
    struct list_item* next;
    void* data;
} list_item_t;

typedef struct list {
    unsigned int count;
    size_t data_size;
    list_item_t* first;
} list_t;

void list_init(list_t* list, size_t data_size);
void list_add(list_t* list, void* data);
void* list_get(list_t* list, unsigned int index);
int list_pop(list_t* list, void* data);
void list_clear(list_t* list);
void list_release(list_t* list);

#endif // LIST_H
