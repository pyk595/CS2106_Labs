#include <stdio.h>
#include <stdlib.h>
#include "llist.h"
#include "mymalloc.h"
#include "bitmap.h"

unsigned char _heap[MEMSIZE] = {0};
unsigned char _bitmap[MAPSIZE] = {0};

TNode *list = NULL;

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((unsigned char *) ptr - &_heap[0]);
}

void print_memlist() {
    print_map(_bitmap, MAPSIZE);
  // Implement this to call print_map from bitmap.c
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {
  long index = search_map(_bitmap, MAPSIZE, size);
  if (index == -1) {
    return NULL;
  } else {
    allocate_map(_bitmap, index, size);
    TData *data = (TData *) malloc(sizeof(TData));
    data->len = size;
    TNode *node = make_node(index, data);
    insert_node(&list, node, 1);
    return &_heap[index];
  }
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
  TNode *node = find_node(list, (unsigned int) get_index(ptr));
  if (node == NULL) {
    return;
  }
  TData *data = node->pdata;
  int index = node->key;
  int size = data->len;
  delete_node(&list, node);
  free(data);
  free_map(_bitmap, index, size);

}

