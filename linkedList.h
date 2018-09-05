#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct linkedList {
    struct elt *head;  /* dequeue this next */
    struct elt *tail;  /* enqueue after this */
}linkedList;

struct elt {
    struct elt *next;
    char *str;
};


linkedList * linkedListCreate(void);

void addToList(struct linkedList *l, char *str);

int searchList(struct linkedList *l, char *str);

void linkedListDestroy(struct linkedList *l, int t);

