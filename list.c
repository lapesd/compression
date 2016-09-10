/*
*/
#define NULL 0
#include <stdio.h>
#include <stdlib.h>

//! A node struct
/*! It is a struct to describe every element in the list*/
struct node {
    struct node* prev;
    struct node* next;
    short val;
};

//! A list struct
/*! It is a struct to describe the control block of a list*/
struct list {
    struct node* first;
    struct node* last;
    int size;
};

//! Function to add a new element in the end of the list
/*! 
 * \param l a pointer to the control block of a list which will add a new element.
 * \param v a short integer which will be added in the end of the list.
*/
void add(struct list* l, short v) {
    struct node* n = malloc(sizeof(* n));
    n->val = v;
    if (l->first == NULL) {
        l->first = n;
        l->last = n;
        n->prev = 0;
        n->next = 0;
    } else {
        n->prev = l->last;
        n->next = NULL;
        l->last->next = n;
        l->last = n;
    }
    l->size++;
}

//! Function to get the value of an element in a given position
/*! 
 * \param list a pointer to the control block of a list which will have the value searched from.
 * \param index an integer which will be used to search into the position index of the list.
 * \return the element in the index position
*/
short get(struct list* list, int index) {
    if(list->size > index) {
        int i;
        struct node* actual;
        actual = list->first;
        for(i = 0; i < index; i++) {
            actual = actual->next;
        }
        return actual->val;
    }
    perror("bad indexing getting an element");
}


//! Function to alloc memory for the control block of a list
/*! 
 * \return a pointer to the list's control block that has been allocated.
*/
struct list* alloc_list() {
    struct list* list = (struct list*) malloc(sizeof(struct list));
    list->first = NULL;
    list->last = NULL;
    return list;
}

//! Function to create a node given a value
/*! 
 * \param value a short integer value to define de node value.
 * \return a pointer to the allocated node.
*/
struct node* alloc_node(short value) {
    struct node* node = (struct node*) malloc(sizeof(struct node));
    node->prev = NULL;
    node->next = NULL;
    node->val = value;
    return node;
}

//! Function the return a list with a defined size and fullfilled with zeroes
/*! 
 * \param size a integer to define the list size.
 * \param v a short integer which will be added in the end of the list.
 * \return a pointer to the list
*/
struct list* zeroes(int size) {
    struct list* list = alloc_list();
    int i;
    for(i = 0; i < size; i++)
        add(list, 0);
    return list;
}

//! Function substitute an element in a given position
/*! 
 * \param list a pointer to the control block of a list.
 * \param index the position which will have its value substitued.
 * \param value the new value of the node
*/
void substitute_at(struct list* list, int index, short value) {
    if(list->size > index) {
        int i;
        struct node* actual;
        actual = list->first;
        for(i = 0; i < index; i++) {
            actual = actual->next;
        }
        actual->val = value;
        return;
    }
    perror("bad indexing at substituing");
}

//! Function to print a list
/*! 
 * \param l a pointer to the control block of a list.
*/
void print_list(struct list* l) {
    struct node *n = l->first;
    printf("[");
    int i;
    for(i = 0; i < l->size; i++) {
        printf("%d", n->val);
        if(i != l->size-1) {
            printf(", ");
        }
        n = n->next;
    }
    printf("]\n");
}

void destroy_list(struct list* list) {
    int i;
    struct node* current = list->last, * aux;
    for(i = list->size; i > 0; i--) {
        aux = current;
        current = aux->prev;
        free(aux);
    }
    free(list);
    list = NULL;
}