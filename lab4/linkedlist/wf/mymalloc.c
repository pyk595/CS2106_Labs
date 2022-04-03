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

void init() {
    if (_memlist == NULL) {
        // add first node
        TData *_data = (TData *) malloc(sizeof(TData));
        _data->occupied = 0;
        _data->len = MEMSIZE;
        TNode *_node = make_node(0, _data);
        insert_node(&_memlist, _node, ASCENDING);
    }
}

void print_node(TNode *node) {
    if(node != NULL) {
        TData *data = node->pdata;
        if (data->occupied)
            printf("Status: ALLOCATED Start index: %u Length: %zu\n", node->key, data->len);
        else
            printf("Status: FREE Start index: %u Length: %zu\n", node->key, data->len);
    }
}

void print_memlist() {
    if (_memlist == NULL) {
        printf("Status: FREE Start index: %u Length: %u\n", 0, MEMSIZE);
    } else {
        reset_traverser(_memlist, FRONT);
        process_list(_memlist, print_node);
    }
}

// Allocates size bytes of memory and returns a pointer
// to the first byte.
void *mymalloc(size_t size) {

    if (size > MEMSIZE)
        return NULL;

    // add first node if _memlist is null
    init();

    reset_traverser(_memlist, FRONT);
    TNode *node;
    TData *data;
    TNode *max_mem_node = NULL;
    size_t max_size;

    do {
        node = succ(_memlist);
        if (node) {
            data = node->pdata;
            if ((!(data->occupied)) && (data->len) >= size) {
                if (max_mem_node == NULL || max_size < (data->len)) {
                    max_mem_node = node;
                    max_size = data->len;
                }
            }
        }
    } while (node != NULL);

    if (!(max_mem_node))
        return NULL;

    node = max_mem_node;
    data = node->pdata;
    data->occupied = 1;

    if (data->len > size) {
        size_t leftover = data->len - size;
        unsigned int start_index = node->key + size;
        data->len = size;

        TData *new_data = (TData *) malloc(sizeof(TData));
        new_data->occupied = 0;
        new_data->len = leftover;
        TNode *new_node = make_node(start_index, new_data);
        insert_node(&_memlist, new_node, ASCENDING);
    }

    return &_heap[node->key];
}

void free_data(TNode *node) {
    free(node->pdata);
}

// Frees memory pointer to by ptr.
void myfree(void *ptr) {
    TNode *node = find_node(_memlist, get_index(ptr));

    TData *data = node->pdata;
    TNode *pred_node = node->prev;
    TNode *succ_node = node->next;
    TData *t_data;

    if (pred_node != NULL) {
        t_data = pred_node->pdata;
        if (!(t_data->occupied)) {
            size_t len = t_data->len + data->len;
            free_data(node);
            merge_node(_memlist, pred_node, SUCCEEDING);
            t_data->len = len;
            node = pred_node;
            data = pred_node->pdata;
        }
    }

    if (succ_node != NULL) {
        t_data = succ_node->pdata;
        if (!(t_data->occupied)) {
            size_t len = t_data->len + data->len;
            free_data(succ_node);
            merge_node(_memlist, node, SUCCEEDING);
            data->len = len;
        }
    }

    data->occupied = 0;
    
    // free the only node if no 
    if (node->key == 0 && data->len == MEMSIZE && !(data->occupied)) {
        free_data(node);
        delete_node(&_memlist, node);
        _memlist = NULL;
    }
}

