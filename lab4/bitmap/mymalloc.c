#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "bitmap.h"
#include "llist.h"

unsigned char _heap[MEMSIZE] = {0};
unsigned char _bitmap[MAPSIZE] = {0};

// linked list to store allocated size, where key is start_index and value is size
TNode *llist = NULL;

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((unsigned char *) ptr - &_heap[0]);
}

void print_memlist() {
    // Implement this to call print_map from bitmap.c
    print_map(_bitmap, MAPSIZE);
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {
    // search for free space
    long start_index = search_map(_bitmap, MAPSIZE, size);
    
    if (start_index == -1) {
        return NULL;
    } else {
        allocate_map(_bitmap, start_index, size);

        // save key: start_index, pdata: (len: size) in a linked list
        TData *data = (TData *) malloc(sizeof(TData));
        data->len = size;
        TNode *node =  make_node(start_index, data);
        insert_node(&llist, node, ASCENDING);
        
        // return simulated heap
        return &_heap[start_index];
    }
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    TNode *node = find_node(llist, (unsigned int) get_index(ptr));
    if (node == NULL)
        return;

    // get start_index and size from linked list
    TData *data = node->pdata;
    int start_index = node->key;
    int size = data->len;

    // release heap memory used by linked list node
    delete_node(&llist, node);
    free(data);
    
    // free bitmap
    free_map(_bitmap, start_index, size);
}

