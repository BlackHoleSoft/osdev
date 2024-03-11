#include "types.h"
#include "string.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void clear();
void print_position(int x, int y);
int print_at(int offset, string str, u8 color);
void print_colored(string str, u8 color);
void print(string str);
void println(string str);
void print_cursor_enable(u8 cursor_start, u8 cursor_end);
void print_cursor_disable();
void print_cursor_set(int x, int y);
