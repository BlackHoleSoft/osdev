#include "types.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void clear();
int str_len(string str);
int print_at(int offset, string str, u8 color);
void print_colored(string str, u8 color);
void print(string str);
void println(string str);
string num_to_str(int num, int r);
string char_to_str(char c);
