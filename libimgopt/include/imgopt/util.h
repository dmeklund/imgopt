#ifndef _IMGOPT_UTIL_H
#define _IMGOPT_UTIL_H

#define GC_THREADS
#include <gc.h>

#define xmalloc GC_MALLOC
#define xrealloc GC_REALLOC
#define xmalloc_atomic GC_MALLOC_ATOMIC

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

enum Error
{
    OK,
    INVALID_ARG
};

struct Array
{
    void **values;
    int count;
    int capacity;
};

struct LinkedListNode
{
    struct LinkedListNode *next;
    void *value;
};

struct LinkedList
{
    struct LinkedListNode *head;
    struct LinkedListNode *tail;
};

extern enum Error linkedlist_appendRight(struct LinkedList *list, void *value);
extern void* linkedlist_popLeft(struct LinkedList *list);
extern enum Error array_append(struct Array *arr, void *value);
extern struct Array* array_new();
extern void array_reverse(struct Array *arr);


#endif /* _IMGOPT_UTIL_H */
