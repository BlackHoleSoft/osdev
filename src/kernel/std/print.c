#include "types.h"
#include "print.h"
#include "mem.h"

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

int str_len(string str) {
    int len;
    for (len = 0; str[len] > 0; len++) { }
    return len;
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

string num_to_str(int num, int r) {
    if (num == 0) return "0";

    string str = mem_512();
    str[15] = 0;
    int n = num;
    int i = 0;   
    int end = str_len(str) - 1;

    while (n > 0) {
        char mod = n % r;
        n /= r;
        str[end - 1 - i] = mod > 9 ? (mod - 10) + 0x61 : mod + 0x30;
        i++;
    }
    str[end] = '\0';
    mem_free(str);
    return (string)((ulong)str + (end - i));
}

string char_to_str(char c) {
    string str = " ";
    str[0] = c;
    return str;
}