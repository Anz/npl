#include "list.h"
#include <stdlib.h>
#include <string.h>

void list_init(list_t* list, size_t data_size) {
    list->count = 0;
    list->data_size = data_size;
    list->first = NULL;
}

void list_add(list_t* list, void* data) {
    void* memory = malloc(sizeof(list_item_t) + list->data_size);

    list_item_t* item = (list_item_t*)memory;
    item->data = memory + sizeof(list_item_t);
    memcpy(item->data, data, list->data_size);

    if (list->count == 0) {
        // if list is empty
        list->first = item;
        item->previous = item;
        item->next = item;
    } else {
        // if list has aleast 1 item
        item->previous = list->first->previous;
        item->next = list->first;
        list->first->previous->next = item;
        list->first->previous = item;
    }
    list->count++;
}

void* list_get(list_t* list, unsigned int index) {
    if (list->count == 0) {
        // list empty
        return NULL;
    }

    list_item_t* item = list->first;
    for (int i = 0; i < list->count; i++) {
        if (i == index) {
            // found
            return item->data;
        }

        // get next
        item = item->next;
    }

    // not found
    return NULL;
}

int list_pop(list_t* list, void* data) {
    if (list->count == 0) {
        return 0;
    }
    list_item_t* first = list->first;
    memcpy(data, first->data, list->data_size);
    list->count--;

    list_item_t* previous = first->previous;
    list_item_t* next = first->next;
    previous->next = next;
    next->previous = previous;
    free(first);
    return list->count + 1;
}

void list_clear(list_t* list) {
    list_release(list);
}

void list_release(list_t* list) {
    // delete all items
    list_item_t* item = list->first;
    for (int i = 0; i < list->count; i++) {
        list_item_t* deletable = item;

        // get next
        item = item->next;

        // delete
        free(deletable);
    }

    // reset list
    list->first = NULL;
    list->count = 0;
    list->data_size = 0;
}
