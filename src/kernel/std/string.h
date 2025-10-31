#include "types.h"

int str_len(string str);
string num_to_str(long num, int r);
string char_to_str(char c);
string str_concat(string buffer, string s1, string s2);
string str_copy(string buffer, string value);
bool str_compare(string str1, string str2);
void* memset(void* ptr, int value, size_t num);
void* memcpy(void* destination, const void* source, size_t num);
