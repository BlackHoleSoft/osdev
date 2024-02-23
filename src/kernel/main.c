#include "std/print.h"
#include "std/io.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

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

void kbd_ack(void){
  while(!(inb(0x60)==0xfa));
}

void kmain() {
    print(SCREEN_WIDTH * 1, "Strelka System");
    print(SCREEN_WIDTH * 2, num_to_str(0xdead666, 16));
    print(SCREEN_WIDTH * 3, num_to_str(666777, 10));
    print(SCREEN_WIDTH * 4, num_to_str(7, 2));

    outb(0x60, 0xed);
    kbd_ack();
    outb(0x60, 0x1);
    outb(0x60, 0x1);

    for (int i=0; i<999999999; i++) {}

    outb(0x60, 0xed);
    kbd_ack();
    outb(0x60, 0x3);
    outb(0x60, 0x3);

    for (int i=0; i<999999999; i++) {}
    outb(0x60, 0xed);
    kbd_ack();
    outb(0x60, 0x0);
    outb(0x60, 0x0);

    print(SCREEN_WIDTH * 6, "outb");

    while (1) {
        char k = inb(0x60);
        print(SCREEN_WIDTH * 7, num_to_str(k, 16));
    }

    // test_mem_in_vm();

}