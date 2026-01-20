#include <stdio.h>
#include "LINKLIST.H"
#define NULL 0

struct node *mknode(n) int n; {
    struct node* x;
    x = alloc(4);
    x->num = n;
    x->next = NULL;
    return x;
}

void dellst(list) struct node* list; {
    struct node* next; struct node* current;
    current = list;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next; 
    }
    list = NULL;
}