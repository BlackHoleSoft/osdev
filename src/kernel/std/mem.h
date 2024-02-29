#include "types.h"

#define MEM_SECTIONS_COUNT 1024
#define MEM_DATA_ADDR 0x1000000 // 16mb offset from start
#define MEM_FIRST_TABLE_ADDR 0x1000100
#define MEM_SECTION_GAP 0x100

struct MemoryTable
{
    struct MemoryTable* next;
    int section_size;
    ulong block_addr;
    bool sections_enabled[MEM_SECTIONS_COUNT];
    ulong memory;
};

struct MemoryTablesList {
    struct MemoryTable* first;
    int count;
    int vga_offset;
};

int mem_get_size();

void mem_init();

int mem_total_size();

int mem_used_size();

void* mem_512();

void* mem_10kb();

bool mem_free(void* ptr);

int mem_get_overall_used();