#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// header of memory region
typedef struct mem_block_header_tag {
  size_t size;
  struct mem_block_header_tag * next;
  struct mem_block_header_tag * prev;
} mem_block_t;

// Global variables
mem_block_t * freemem_list_head = NULL;
unsigned long heap_size = 0;
unsigned long free_list_size = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Helper functions
void remove_from_freemem_list(mem_block_t * freemem_ptr);
void add_to_freemem_list(mem_block_t * freemem_ptr, size_t freemem_size);
void * split_freemem(mem_block_t * freemem_ptr, size_t size);
void * Allocate(mem_block_t * freemem_ptr, size_t size);

// Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

// Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

// performance study
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
