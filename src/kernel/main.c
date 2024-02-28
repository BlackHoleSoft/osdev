#include "std/print.h"
#include "std/io.h"
#include "std/keyboard.h"
#include "std/mem.h"
#include "std/ata.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

struct MemGlobal {
    u8 kbd_keycode;
    u32 reserved;
};

struct StateGlobal {
    char symbol;
    bool initialized;
};

void user_loop(struct MemGlobal* mem, struct StateGlobal* state, struct StateGlobal* prev) {

    // logic    

    if (state->initialized == false) {
        state->symbol = kbd_symbol(mem->kbd_keycode);

        if (prev->symbol != state->symbol) {
            state->initialized = true;
        }
    }    

    //render

    if (state->initialized == false) {
        print(SCREEN_WIDTH * 6, "Enter:");
    } else {        
        //print(SCREEN_WIDTH * 6, "Symbol:");
        print(SCREEN_WIDTH * 6 + 9, char_to_str(state->symbol));
    }
}

void sys_loop(struct MemGlobal* mem) {
    mem->kbd_keycode = kbd_keycode();
}

void test_numbers() {
    void* test = (void*)(((struct MemoryTable*)MEM_FIRST_TABLE_ADDR)->memory);

    print(SCREEN_WIDTH * 3, num_to_str(666, 10));
    print(SCREEN_WIDTH * 4, num_to_str(-666, 10));
    print(SCREEN_WIDTH * 5, num_to_str(2000000000, 10));
    print(SCREEN_WIDTH * 6, num_to_str(true, 10));
    print(SCREEN_WIDTH * 7, num_to_str(false, 10));
    print(SCREEN_WIDTH * 8, num_to_str((int)test, 16));
    print(SCREEN_WIDTH * 9, num_to_str((int)NULL, 10));
}

void test_mem() {
    string test512 = mem_512();
    for (int i = 0; i<512; i++) {
        test512[i] = 'q';
    }
    test512[8] = '\0';

    string test10kb = mem_10kb();
    if (test10kb == NULL)
        print(SCREEN_WIDTH * 10, "test10kb is NULL!");

    for (int i = 0; i<512; i++) {
        test10kb[i] = 'w';
    }
    test10kb[8] = '\0';

    print(SCREEN_WIDTH * 4, test512);
    print(SCREEN_WIDTH * 5, test10kb);

    print(SCREEN_WIDTH * 7 + 0, num_to_str(mem_used_size(), 10));
    print(SCREEN_WIDTH * 7 + 12, "/");
    print(SCREEN_WIDTH * 7 + 14, num_to_str(mem_total_size(), 10));

    print(SCREEN_WIDTH * 8 + 0, num_to_str((int)test512, 10));

    print(SCREEN_WIDTH * 8 + 50, num_to_str((int)(string)mem_512(), 10));
    print(SCREEN_WIDTH * 8 + 65, num_to_str((int)(string)mem_512(), 10));

    print(SCREEN_WIDTH * 8 + 16, num_to_str((int)test10kb, 10));

    print(SCREEN_WIDTH * 8 + 32, num_to_str(MEM_FIRST_TABLE_ADDR, 10));
    
    print(SCREEN_WIDTH * 9, num_to_str(mem_free(test512), 10));
    print(SCREEN_WIDTH * 9 + 8, num_to_str(mem_free(test10kb), 10));
    
    print(SCREEN_WIDTH * 11, num_to_str((int)mem_512(), 10));
    print(SCREEN_WIDTH * 11 + 20, num_to_str((int)mem_512(), 10));
    print(SCREEN_WIDTH * 11 + 40, num_to_str((int)mem_512(), 10));

    print(SCREEN_WIDTH * 10 + 0, num_to_str(mem_used_size(), 10));
}

void test_disk() {
    string contents = mem_512();
    print(SCREEN_WIDTH * 1, num_to_str((int)contents, 16));

    ata_read_sectors(contents, 0x3, 1);

    for (int i = 0; i<512; i++) {
        print(SCREEN_WIDTH*3 + i, char_to_str(contents[i]));
    }
    
    contents[0] = contents[0] < 'a' ? 'a' : contents[0] + 1;
    contents[1] = '!';
    contents[2] = '\0';

    ata_write_sectors(0x3, 1, contents);

    // ata_read_sectors(contents, 0x3, 1);
    // for (int i = 0; i<512; i++) {
    //     print(SCREEN_WIDTH*3 + i, char_to_str(contents[i]));
    // }
    
    mem_free(contents);
}

void kmain() {
    print(SCREEN_WIDTH * 1, "Strelka System");

    //kbd_leds(0x3);

    for (int i=0; i<999999999; i++) {}

    //kbd_leds(0x0);

    int memsize = mem_get_size();
    print(SCREEN_WIDTH * 2 + 0, "Memory(Mb):     / ");
    print(SCREEN_WIDTH * 2 + 12, num_to_str(mem_get_overall_used() / 1024 / 1024, 10));
    print(SCREEN_WIDTH * 2 + 18, memsize > 0 ? num_to_str(memsize / 1024 / 1024, 10) : ">=4Gb");

    mem_init();

    // struct MemGlobal mem_global;
    // struct StateGlobal state_global;
    // struct StateGlobal state_global_prev;

    // state_global.symbol = 0;
    // state_global.initialized = false;

    // state_global_prev.symbol = 0;
    // state_global_prev.initialized = false;

    // while (1) {
    //     sys_loop(&mem_global);
    //     user_loop(&mem_global, &state_global, &state_global_prev);

    //     state_global_prev.symbol = state_global.symbol;
    //     state_global_prev.initialized = state_global.initialized;

    //     u8 kc = mem_global.kbd_keycode;
    //     print(SCREEN_WIDTH * 12, num_to_str(kc, 16));
    //     print(SCREEN_WIDTH * 13, char_to_str(kbd_symbol(kc)));
    // }

    test_numbers();

    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    clear();

    test_mem();

    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    clear();

    test_disk();

}