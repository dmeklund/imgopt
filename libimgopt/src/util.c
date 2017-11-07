#include "imgopt/util.h"

#include <stdio.h>
#include <assert.h>


extern enum Error linkedlist_appendRight(struct LinkedList *list, void *value)
{
    assert(value != NULL);
    struct LinkedListNode *node = xmalloc(sizeof *node);
    node->value = value;
    node->next = NULL;
    if (list->tail == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    return OK;
}


extern void* linkedlist_popLeft(struct LinkedList *list)
{
    if (list->head == NULL) return NULL;
    void *result = list->head->value;
    assert(result != NULL);
    list->head = list->head->next;
    if (list->head == NULL)
        list->tail = NULL;
    return result;
}


extern enum Error array_append(struct Array *arr, void *value)
{
    assert (value != NULL);
    if (arr->count == arr->capacity)
    {
        arr->capacity = 2*arr->capacity+1;
        arr->values = xrealloc(arr->values, arr->capacity * sizeof *arr->values);
    }
    arr->values[arr->count] = value;
    arr->count += 1;
    return OK;
}


extern struct Array* array_new()
{
    struct Array *arr = xmalloc(sizeof *arr);
    *arr = (struct Array){
            .count = 0,
            .capacity = 4,
            .values = xmalloc(4 * sizeof *arr->values)
    };
    return arr;
}


extern void array_reverse(struct Array *arr)
{
    for (int index = 0; index < arr->count - index; ++index)
    {
        void *swap_val = arr->values[index];
        arr->values[index] = arr->values[arr->count - index];
        arr->values[arr->count - index] = swap_val;
    }
}
