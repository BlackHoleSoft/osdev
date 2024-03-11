#include "types.h"
#include "print.h"
#include "mem.h"
#include "io.h"

void print_init() {
    struct MemoryTablesList* data = (struct MemoryTablesList*)MEM_DATA_ADDR;
    data->vga_offset = 0;
}

void clear() {
    struct MemoryTablesList* data = (struct MemoryTablesList*)MEM_DATA_ADDR;
    data->vga_offset = 0;

    char* vid = (char*)0xb8000;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i++) {
        vid[i * 2 + 0] = 0;
        vid[i * 2 + 1] = 2;
    }
}

void print_position(int x, int y) {
    struct MemoryTablesList* data = (struct MemoryTablesList*)MEM_DATA_ADDR;

    data->vga_offset = y * SCREEN_WIDTH + x;
}

int print_at(int offset, string str, u8 color) {
    char* vid = (char*)0xb8000;

    int i;
    for (i = 0; str[i] > 0; i++, offset++) {
        if (str[i] == '\n') {
            offset += SCREEN_WIDTH - offset % SCREEN_WIDTH - 1;
            continue;
        }

        vid[offset * 2 + 0] = str[i];
        vid[offset * 2 + 1] = color;
    }
    return offset;
}

void print_colored(string str, u8 color) {
    struct MemoryTablesList* data = (struct MemoryTablesList*)MEM_DATA_ADDR;
    data->vga_offset = print_at(data->vga_offset, str, color);
}

void print(string str) {
    print_colored(str, 0x2);
}

void println(string str) {
    print(str);
    print("\n");
}

void print_cursor_enable(u8 cursor_start, u8 cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void print_cursor_disable()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void print_cursor_set(int x, int y)
{
	u16 pos = y * SCREEN_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}