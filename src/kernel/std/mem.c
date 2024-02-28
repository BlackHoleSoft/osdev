#include "mem.h"

struct MemoryTable* get_block(int index) {
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct MemoryTable* mem_table = mem_list->first;
    for (int i=1; i<=index; i++) {
        if ((void*)(mem_table->next) == NULL)
            return NULL;
        mem_table = mem_table->next;
    }
    return mem_table;
}

bool try_mem(ulong addr) {
    char* ptr = (char*) addr;
    ptr[0] = 'M';
    return ptr[0] == 'M';
}

int mem_get_size() {
    for (int i = 0; i <= 4096; i += 32) {        
        if (!try_mem(0x100000 * i)) {
            return 0x100000 * i;
        }
    }
    return 0;
}

void* mem_section(int index) {
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;

    if (index >= mem_list->count) return NULL;

    // find memory block
    struct MemoryTable* mem_table = get_block(index);

    // find first unused section in block
    for (int i=0; i<MEM_SECTIONS_COUNT; i++) {
        if (mem_table->sections_enabled[i] == false) {
            mem_table->sections_enabled[i] = true;
            // only get ADDRESS of memory field, not a value !!!
            return (void*)((ulong)&mem_table->memory + (ulong)i * mem_table->section_size);
        }
    }
    return NULL;
}

bool mem_free(void* ptr) {
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct MemoryTable* mem_table = mem_list->first;
    
    while (mem_table != NULL) {
        // only get ADDRESS of memory field, not a value !!!
        ulong offset = (ulong)ptr - (ulong)(&(mem_table->memory));

        if (offset < mem_table->section_size * MEM_SECTIONS_COUNT) {
            int index = offset / mem_table->section_size;
            mem_table->sections_enabled[index] = false;
            return true;
        }        

        mem_table = mem_table->next;
    }
    return false;
}

void mem_block_init(int section_size) {
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;    
    struct MemoryTable* mem_table = get_block(mem_list->count - 1);

    ulong prev_block_size = mem_table->section_size * MEM_SECTIONS_COUNT + MEM_SECTION_GAP;
    ulong block_addr = mem_table->block_addr + prev_block_size;
    struct MemoryTable* new_table = (struct MemoryTable*)block_addr;
    new_table->next = NULL;
    new_table->section_size = section_size;
    new_table->block_addr = block_addr;
    new_table->memory = (ulong)(&(new_table->memory));

    for (int i=0; i<MEM_SECTIONS_COUNT; i++) {
        new_table->sections_enabled[i] = false;
    }

    mem_table->next = new_table;
    mem_list->count += 1;
}

// main initialization of memory mapping table
void mem_init() {
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct MemoryTable* first_table = (struct MemoryTable*)MEM_FIRST_TABLE_ADDR;

    first_table->section_size = 512;
    first_table->next = NULL;
    first_table->block_addr = MEM_FIRST_TABLE_ADDR;
    first_table->memory = (ulong)(&(first_table->memory));
    
    for (int i=0; i<MEM_SECTIONS_COUNT; i++) {
        first_table->sections_enabled[i] = false;
    }

    mem_list->count = 1;
    mem_list->first = first_table;

    mem_block_init(1024 * 10); // 10 mb block on index=1    
}

int mem_total_size() {
    int size = 0;
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct MemoryTable* mem_table = mem_list->first;
    while (mem_table != NULL) {        
        size += mem_table->section_size * MEM_SECTIONS_COUNT + MEM_SECTION_GAP;
        mem_table = mem_table->next;
    }
    return size;
}

int mem_used_size() {
    int size = 0;
    struct MemoryTablesList* mem_list = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct MemoryTable* mem_table = mem_list->first;
    while (mem_table != NULL) {     
        int used = 0;
        for (int j=0; j<MEM_SECTIONS_COUNT; j++) {
            if (mem_table->sections_enabled[j])
                used += mem_table->section_size;
        }   
        size += used + MEM_SECTION_GAP;
        mem_table = mem_table->next;
    }
    return size;
}

void* mem_512() {
    return mem_section(0);
}

void* mem_10kb() {
    return mem_section(1);
}

int mem_get_overall_used() {
    return mem_used_size() + MEM_FIRST_TABLE_ADDR;
}