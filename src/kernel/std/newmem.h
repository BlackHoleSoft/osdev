#ifndef NEWMEM_H
#define NEWMEM_H

#include "types.h"

// Memory block header structure
struct MemBlockHeader {
    size_t size;              // Size of the data block
    bool is_free;             // Whether this block is free
    struct MemBlockHeader* next; // Next block in the list
};

// Static structure to hold heap information
struct HeapInfo {
    void* start_addr;         // Start address of the heap
    size_t size;              // Total size of the heap
    struct MemBlockHeader* first_block; // First block in the heap
};

// Initialize the heap
void newmem_init(void* start_addr, size_t size);

// Standard memory allocation functions
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

// Utility functions
size_t get_heap_used();
size_t get_heap_free();

#endif
