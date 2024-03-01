#include "types.h"
#include "string.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void clear();
int print_at(int offset, string str, u8 color);
void print_colored(string str, u8 color);
void print(string str);
void println(string str);

