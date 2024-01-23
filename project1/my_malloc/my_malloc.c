#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

static mem_block_t * freemem_list_head = NULL;
static unsigned long heap_size = 0;
static unsigned long free_list_size = 0;

// this function remove a region from the linked list of free memory regions
void remove_from_freemem_list(mem_block_t * freemem_ptr){
  if(freemem_ptr->next && freemem_ptr->prev){
    freemem_ptr->prev->next = freemem_ptr->next;
    freemem_ptr->next->prev = freemem_ptr->prev;
  }
  else if(!freemem_ptr->next && !freemem_ptr->prev){
    freemem_list_head = NULL;
  }
  else if(!freemem_ptr->next){
    freemem_ptr->prev->next = NULL;
  }
  else{
    freemem_ptr->next->prev = NULL;
    freemem_list_head = freemem_ptr->next;
  }
}

// this function add a region to the linked list of free memory regions
void add_to_freemem_list(mem_block_t * freemem_ptr, size_t freemem_size){
  freemem_ptr->size = freemem_size;
  
  if(!freemem_list_head){
    freemem_list_head = freemem_ptr;
    freemem_list_head->next = NULL;
    freemem_list_head->prev = NULL;
  }
  else{
    mem_block_t * freemem_list_ptr = freemem_list_head;
    if(freemem_list_head > freemem_ptr){
      freemem_ptr->prev = NULL;
      freemem_ptr->next = freemem_list_ptr;
      freemem_list_ptr->prev = freemem_ptr;
      freemem_list_head = freemem_ptr;
    }
    else{
      while(freemem_list_ptr->next){
	if(freemem_list_ptr->next < freemem_ptr){
	  freemem_list_ptr = freemem_list_ptr->next;
	  continue;
	}
	else{
	  freemem_list_ptr->next->prev = freemem_ptr;
	  freemem_ptr->next = freemem_list_ptr->next;
	  freemem_ptr->prev = freemem_list_ptr;
	  freemem_list_ptr->next = freemem_ptr;
	  return;
	}
      }
      freemem_list_ptr->next = freemem_ptr;
      freemem_ptr->prev = freemem_list_ptr;
      freemem_ptr->next = NULL;
    }
  }
}

// this function split a free memory region to two smaller pieces
// return one of them, and add another one to free memory list
void * split_freemem(mem_block_t * freemem_ptr, size_t size){
  void * allocated_addr;
  mem_block_t * splited_freemem_ptr;
  size_t splited_freemem_size;

  allocated_addr = (void*)freemem_ptr + sizeof(mem_block_t);
  splited_freemem_ptr = (void*)freemem_ptr + sizeof(mem_block_t) + size;
  splited_freemem_size = freemem_ptr->size - size - sizeof(mem_block_t);
  freemem_ptr->size = size;
  remove_from_freemem_list(freemem_ptr);
  add_to_freemem_list(splited_freemem_ptr, splited_freemem_size);

  return allocated_addr;
}

// this function allocate memory from a free memory region
void * allocate(mem_block_t * freemem_ptr, size_t size){
  void * allocated_addr;
  // if this block's size perfectly matches the size we want to allocate
  if(freemem_ptr->size > size + sizeof(mem_block_t)){
    allocated_addr = split_freemem(freemem_ptr, size);
    free_list_size = free_list_size - size - sizeof(mem_block_t);
  }
  else{
    remove_from_freemem_list(freemem_ptr);
    allocated_addr = (void*)freemem_ptr + sizeof(mem_block_t);
    free_list_size = free_list_size - freemem_ptr->size - sizeof(mem_block_t);
  }
  return allocated_addr;
}

// First Fit malloc
void * ff_malloc(size_t size){

  void * allocated_addr; // the ptr we return
  mem_block_t * freemem_list_ptr = freemem_list_head; // ptr to list of free memory regions
  size_t actual_alloc_size = size + sizeof(mem_block_t); // the actual allocated size (block size + header size)
  mem_block_t * newspace_ptr; // ptr to space created by sbrk()

  while(freemem_list_ptr){ // when there are memory can be allocated
    if(freemem_list_ptr->size >= size){
      allocated_addr = allocate(freemem_list_ptr, size);
      return allocated_addr;
    }
    else{
      freemem_list_ptr = freemem_list_ptr->next;
    }
  }
  // if we go through all the memory in free list but still cannot find enough space, we use sbrk() to extend heap
  newspace_ptr = sbrk(actual_alloc_size); // system page size
  newspace_ptr->size = size;
  newspace_ptr->next = NULL;
  newspace_ptr->prev = NULL;
  heap_size += actual_alloc_size;
  allocated_addr = (void*)newspace_ptr + sizeof(mem_block_t);
  return allocated_addr;
}

// First Fit free
void ff_free(void * ptr){
  // move the ptr back to header
  mem_block_t * curr_mem = ptr - sizeof(mem_block_t);
  free_list_size = free_list_size + curr_mem->size + sizeof(mem_block_t);
  // first step, we add this memory region to the linked list of free memory regions
  add_to_freemem_list(curr_mem, curr_mem->size);
  mem_block_t * prev_mem = curr_mem->prev;
  mem_block_t * next_mem = curr_mem->next;
  size_t header_size = sizeof(mem_block_t);
  // second step, we merge the newly freed region with any currently free adjacent regions
  if(prev_mem == NULL){ 
    if(next_mem == NULL){ 
      return;
    }
    else if((size_t)curr_mem + header_size + curr_mem->size == (size_t)next_mem){
      curr_mem->size += header_size + next_mem->size;
      remove_from_freemem_list(next_mem);
    }
  }
  else if(next_mem == NULL){
    if((size_t)prev_mem + header_size + prev_mem->size == (size_t)curr_mem){
      prev_mem->size += header_size + curr_mem->size;
      remove_from_freemem_list(curr_mem);
    }
  }
  else if((size_t)prev_mem + header_size + prev_mem->size == (size_t)curr_mem && (size_t)curr_mem + header_size + curr_mem->size == (size_t)next_mem){
      prev_mem->size += 2 * header_size + curr_mem->size + next_mem->size;
      remove_from_freemem_list(curr_mem);
      remove_from_freemem_list(next_mem);
  }
  else if((size_t)curr_mem + header_size + curr_mem->size == (size_t)next_mem){
    curr_mem->size += header_size + next_mem->size;
    remove_from_freemem_list(next_mem);
  }
  else if((size_t)prev_mem + header_size + prev_mem->size == (size_t)curr_mem){
    prev_mem->size += header_size + curr_mem->size;
    remove_from_freemem_list(curr_mem);
  }
}

void * bf_malloc(size_t size){
  void * allocated_addr; // the ptr we return
  mem_block_t * freemem_list_ptr = freemem_list_head; // ptr to list of free memory regions
  size_t actual_alloc_size = size + sizeof(mem_block_t); // the actual allocated size (block size + header size)
  mem_block_t * newspace_ptr; // ptr to space created by sbrk()
  mem_block_t * best_fit_ptr = NULL;
  while(freemem_list_ptr){ // when there are memory can be allocated
    if(freemem_list_ptr->size == size){
      allocated_addr = allocate(freemem_list_ptr, size);
      return allocated_addr;
    }
    else if(freemem_list_ptr->size > size){
      if(best_fit_ptr == NULL){
	best_fit_ptr = freemem_list_ptr;
	freemem_list_ptr = freemem_list_ptr->next;
	continue;
      }
      else{
	if(freemem_list_ptr->size < best_fit_ptr->size){
	  best_fit_ptr = freemem_list_ptr;
	  freemem_list_ptr = freemem_list_ptr->next;
	  continue;
	}
	else{
	  freemem_list_ptr = freemem_list_ptr->next;
	}
      }
    }
    else{
      freemem_list_ptr = freemem_list_ptr->next;
    }
  }
  if(best_fit_ptr != NULL){
    allocated_addr = allocate(best_fit_ptr, size);
  }
  else{
    newspace_ptr = sbrk(actual_alloc_size); // system page size
    newspace_ptr->size = size;
    newspace_ptr->next = NULL;
    newspace_ptr->prev = NULL;
    heap_size += actual_alloc_size;
    allocated_addr = (void*)newspace_ptr + sizeof(mem_block_t);
  }
  return allocated_addr;
}

void bf_free(void * ptr){
  return ff_free(ptr);
}

unsigned long get_data_segment_size(){
  return heap_size;
}

unsigned long get_data_segment_free_space_size(){
  return free_list_size;
}

int main(int argc, char * argv[]) {
  /*int *dynamicIntArray0 = ff_malloc(4*sizeof(int));
  int *dynamicIntArray1 = ff_malloc(16*sizeof(int));
  int *dynamicIntArray2 = ff_malloc(18*sizeof(int));
  int *dynamicIntArray3 = ff_malloc(32*sizeof(int));
  ff_free(dynamicIntArray0);
  ff_free(dynamicIntArray2);
  int *dynamicIntArray4 = ff_malloc(7*sizeof(int));
  int *dynamicIntArray5 = ff_malloc(256*sizeof(int));
  ff_free(dynamicIntArray5);
  ff_free(dynamicIntArray1);
  ff_free(dynamicIntArray3);
  int *dynamicIntArray6 = ff_malloc(23*sizeof(int));
  int *dynamicIntArray7 = ff_malloc(4*sizeof(int));
  ff_free(dynamicIntArray4);
  int *dynamicIntArray8 = ff_malloc(10*sizeof(int));
  int *dynamicIntArray9 = ff_malloc(32*sizeof(int));
  ff_free(dynamicIntArray6);
  ff_free(dynamicIntArray7);
  ff_free(dynamicIntArray8);
  ff_free(dynamicIntArray9);
  */
  int *dynamicIntArray0 = bf_malloc(4*sizeof(int));
  int *dynamicIntArray1 = bf_malloc(16*sizeof(int));
  int *dynamicIntArray2 = bf_malloc(18*sizeof(int));
  int *dynamicIntArray3 = bf_malloc(32*sizeof(int));
  bf_free(dynamicIntArray0);
  bf_free(dynamicIntArray2);
  int *dynamicIntArray4 = bf_malloc(7*sizeof(int));
  int *dynamicIntArray5 = bf_malloc(256*sizeof(int));
  bf_free(dynamicIntArray5);
  bf_free(dynamicIntArray1);
  bf_free(dynamicIntArray3);
  int *dynamicIntArray6 = bf_malloc(23*sizeof(int));
  int *dynamicIntArray7 = bf_malloc(4*sizeof(int));
  bf_free(dynamicIntArray4);
  int *dynamicIntArray8 = bf_malloc(10*sizeof(int));
  int *dynamicIntArray9 = bf_malloc(32*sizeof(int));
  bf_free(dynamicIntArray6);
  bf_free(dynamicIntArray7);
  bf_free(dynamicIntArray8);
  bf_free(dynamicIntArray9);
  return EXIT_SUCCESS;
  }
