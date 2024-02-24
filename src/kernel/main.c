#include "std/print.h"
#include "std/io.h"
#include "std/keyboard.h"

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

char try_mem(ulong addr) {
    char* ptr = (char*) addr;
    return ptr[0];
}

void test_mem_in_vm() {
    for (int i = 0; i <= 1024; i += 64) {
        char c = try_mem(0x100000 * i);
        char cstr[] = {c, '\0'};
        print(SCREEN_WIDTH * 2 + i / 16, num_to_str(i, 10));
        print(SCREEN_WIDTH * 3 + i / 16, cstr);
    }
}

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

void kmain() {
    print(SCREEN_WIDTH * 1, "Strelka System");

    kbd_leds(0x3);

    for (int i=0; i<999999999; i++) {}

    kbd_leds(0x0);

    struct MemGlobal mem_global;
    struct StateGlobal state_global;
    struct StateGlobal state_global_prev;

    state_global.symbol = 0;
    state_global.initialized = false;

    state_global_prev.symbol = 0;
    state_global_prev.initialized = false;

    while (1) {
        sys_loop(&mem_global);
        user_loop(&mem_global, &state_global, &state_global_prev);

        state_global_prev.symbol = state_global.symbol;
        state_global_prev.initialized = state_global.initialized;

        u8 kc = mem_global.kbd_keycode;
        print(SCREEN_WIDTH * 12, num_to_str(kc, 16));
        print(SCREEN_WIDTH * 13, char_to_str(kbd_symbol(kc)));
    }

    // test_mem_in_vm();

}