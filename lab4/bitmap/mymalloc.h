#define MEMSIZE 64         // Size of memory in bytes
#define MAPSIZE 8
// Used by the test harness
long get_index(void *ptr);

// Debugging routine
void print_memlist();

void *mymalloc(size_t);
void myfree(void *);
