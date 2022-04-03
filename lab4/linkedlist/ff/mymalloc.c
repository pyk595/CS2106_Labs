#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#include "llist.h"

char _heap[MEMSIZE] = {0};
TNode *_memlist = NULL; // To maintain information about length

// Do not change this. Used by the test harness.
// You may however use this function in your code if necessary.
long get_index(void *ptr) {
    if(ptr == NULL)
        return -1;
    else
        return (long) ((char *) ptr - &_heap[0]);
}

void print_node(TNode *node) {
  TData *data = node->pdata;
  if (data->val) {
    printf("Status: FREE Start index: %u Length: %u\n", node->key, (unsigned int) data->size);
  } else {
    printf("Status: ALLOCATED Start index: %u Length: %u\n", node->key, (unsigned int) data->size);
  }
}

void print_memlist() {
  if (_memlist == NULL) {
    printf("Status: FREE Start index: %u Length: %u\n", 0, MEMSIZE);
  } else {
    process_list(_memlist, print_node);
  }
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {
  if (_memlist == NULL) {
    if (size > MEMSIZE) {
      return NULL;
    }
    reset_traverser(_memlist, FRONT);
    TData *data = (TData *) malloc(sizeof(TData));
    data->size = size;
    data->val = 0;
    TNode *node = make_node(0, data);
    insert_node(&_memlist, node, ASCENDING);
    
    TData *unusedData = (TData *) malloc(sizeof(TData));
    unusedData->size = MEMSIZE - size;
    unusedData->val = 1;
    TNode *freeNode = make_node(node->key + (unsigned int) size, unusedData);
    insert_node(&_memlist, freeNode, ASCENDING);
    return &_heap[0];
  } else {
    TNode *current = _memlist;
    int count = 1;
    while (current != NULL) {
      TData *data = current->pdata;

      if (data->val) {
        if (data->size >= size) {
          size_t currSize = data->size;
          unsigned int currStart = current->key;
          free(data);
          delete_node(&_memlist, current);

          TData *newData = (TData *) malloc(sizeof(TData));
          newData->size = size;
          newData->val = 0;
          TNode *node = make_node(currStart, newData);
          insert_node(&_memlist, node, ASCENDING);

          if (currSize > size) {
            TData *unusedData = (TData *) malloc(sizeof(TData));
            unusedData->size = currSize - size;
            unusedData->val = 1;
            TNode *freeNode = make_node(currStart + (unsigned int) size, unusedData);
            insert_node(&_memlist, freeNode, ASCENDING);
          }
          return &_heap[currStart];
        }
      }
      current = current->next;
    }

    return NULL;
  }
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
  TNode *node = find_node(_memlist, (unsigned int) get_index(ptr));
  if (node == NULL) {
    return;
  }
  TData *currData = node->pdata;
  TNode *prev = node->prev;
  TNode *successor = node->next;
  if (prev != NULL) {
    TData *prevData = prev->pdata;
    if (prevData->val) {
      prevData->size = prevData->size + currData->size;
      free(node->pdata);
      merge_node(_memlist, node, PRECEDING);
      currData = prevData;
      node = prev;
    }
  }
  if (successor != NULL) {
    TData *succData = successor->pdata;
    if (succData->val) {
      currData->size = currData->size + succData->size;
      free(succData);
      merge_node(_memlist, node, SUCCEEDING);
    } 
  }
  currData->val = 1;
  if(node->next == NULL && node->pdata->size == MEMSIZE) {
    if (node->pdata->val) {
      free(node->pdata);
      delete_node(&_memlist, node);
      _memlist = NULL;
    }
  }
}

