#include <unistd.h>

// header of memory region
typedef struct mem_block_header_tag {
  size_t size;
  struct mem_block_header_tag * next;
  struct mem_block_header_tag * prev;
} mem_block_t;

// First Fit malloc/free
void *ff_malloc(size_t size);
void ff_free(void *ptr);

// Best Fit malloc/free
void *bf_malloc(size_t size);
void bf_free(void *ptr);

// performance study
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
