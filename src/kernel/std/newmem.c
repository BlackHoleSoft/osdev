#include "newmem.h"
#include "string.h"
#include "print.h"

// Static heap information structure
static struct HeapInfo heap_info = {0};

// Helper function to split a block if it's larger than needed
static void split_block(struct MemBlockHeader* block, size_t size) {
    if (block->size > size + sizeof(struct MemBlockHeader)) {
        struct MemBlockHeader* new_block = (struct MemBlockHeader*)((char*)block + sizeof(struct MemBlockHeader) + size);
        new_block->size = block->size - sizeof(struct MemBlockHeader) - size;
        new_block->is_free = true;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

// Helper function to merge adjacent free blocks
static void merge_blocks(struct MemBlockHeader* current) {
    struct MemBlockHeader* next = current->next;
    
    if (next != NULL && next->is_free && current->is_free) {
        current->size = current->size + sizeof(struct MemBlockHeader) + next->size;
        current->next = next->next;
    }
}

// Initialize the heap with a given start address and size
void newmem_init(void* start_addr, size_t size) {
    if (start_addr == NULL || size < sizeof(struct MemBlockHeader)) {
        return;
    }
    
    heap_info.start_addr = start_addr;
    heap_info.size = size;
    
    // Initialize the first block
    heap_info.first_block = (struct MemBlockHeader*)start_addr;
    heap_info.first_block->size = size - sizeof(struct MemBlockHeader);
    heap_info.first_block->is_free = true;
    heap_info.first_block->next = NULL;
}

// Allocate memory of given size
void* malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    // Add space for header and align size to 8 bytes for better performance
    size_t aligned_size = (size + 7) & ~7;
    
    struct MemBlockHeader* current = heap_info.first_block;
    
    // Find a free block that is large enough
    while (current != NULL) {
        if (current->is_free && current->size >= aligned_size) {
            // Split the block if it's larger than needed
            split_block(current, aligned_size);
            
            // Mark the block as used
            current->is_free = false;
            
            // Return pointer to the data area
            return (void*)((char*)current + sizeof(struct MemBlockHeader));
        }
        current = current->next;
    }
    
    // No suitable block found
    return NULL;
}

// Allocate and zero-initialize memory for num elements of size bytes each
void* calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = malloc(total_size);
    
    if (ptr != NULL) {
        // Zero out the allocated memory
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

// Resize the memory block pointed to by ptr to the given size
void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    // Align size to 8 bytes
    size_t aligned_size = (size + 7) & ~7;
    
    // Get the header for the current block
    struct MemBlockHeader* header = (struct MemBlockHeader*)((char*)ptr - sizeof(struct MemBlockHeader));
    
    if (aligned_size <= header->size) {
        // New size is smaller or equal, we might be able to shrink
        if (aligned_size < header->size) {
            split_block(header, aligned_size);
        }
        return ptr;
    } else {
        // New size is larger, need to allocate new memory and copy
        void* new_ptr = malloc(size);
        if (new_ptr != NULL) {
            // Copy the old data to the new location
            memcpy(new_ptr, ptr, header->size);
            free(ptr); // Free the old memory
        }
        return new_ptr;
    }
}

// Free the memory block pointed to by ptr
void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // Calculate header address
    struct MemBlockHeader* header = (struct MemBlockHeader*)((char*)ptr - sizeof(struct MemBlockHeader));
    
    // Basic validation: check if the header is within our heap range
    if ((char*)header < (char*)heap_info.start_addr || 
        (char*)header >= ((char*)heap_info.start_addr + heap_info.size)) {
        return; // Invalid pointer not in our heap
    }
    
    // Additional validation: check if this header is part of our linked list
    struct MemBlockHeader* current = heap_info.first_block;
    bool found = false;
    
    while (current != NULL) {
        if (current == header) {
            found = true;
            break;
        }
        current = current->next;
    }
    
    // Only free if the header is part of our linked list and currently allocated
    if (found && !header->is_free) {
        // Mark the block as free
        header->is_free = true;
        
        // Try to merge with next block if it's also free
        merge_blocks(header);
    }
}

// Get the amount of heap memory used
size_t get_heap_used() {
    size_t used = 0;
    struct MemBlockHeader* current = heap_info.first_block;
    
    while (current != NULL) {
        if (!current->is_free) {
            used += current->size + sizeof(struct MemBlockHeader);
        }
        current = current->next;
    }
    
    return used;
}

// Get the amount of heap memory free
size_t get_heap_free() {
    size_t free = 0;
    struct MemBlockHeader* current = heap_info.first_block;
    
    while (current != NULL) {
        if (current->is_free) {
            free += current->size + sizeof(struct MemBlockHeader);
        }
        current = current->next;
    }
    
    return free;
}

void print_blocks() {
    struct MemBlockHeader* current = heap_info.first_block;
    char buffer[1024]; // Temporary buffer to build the output string
    int pos = 0;
    
    while (current != NULL) {
        // Format the current block information
        char temp[64]; // Buffer for individual block info
        int len = 0;
        
        // Convert current->size to string (simple integer to string conversion)
        size_t size = current->size;
        if (size == 0) {
            temp[len++] = '0';
        } else {
            char temp_size[20];
            int temp_pos = 0;
            while (size > 0) {
                temp_size[temp_pos++] = '0' + (size % 10);
                size /= 10;
            }
            // Reverse the digits
            for (int i = temp_pos - 1; i >= 0; i--) {
                temp[len++] = temp_size[i];
            }
        }
        
        temp[len++] = ';';
        
        // Add is_free value (0 or 1)
        temp[len++] = '0' + current->is_free;
        
        // Add arrow if there's a next block
        if (current->next != NULL) {
            temp[len++] = '-';
            temp[len++] = '>';
        } else {
            temp[len] = '\0'; // Null terminate for the last block
        }
        
        // Copy the formatted block info to the main buffer
        for (int i = 0; i < len; i++) {
            buffer[pos++] = temp[i];
        }
        
        current = current->next;
    }
    
    // Null terminate the final string
    buffer[pos] = '\0';
    
    // Print the final string
    print(buffer);
    println("");
}
