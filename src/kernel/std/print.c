#include "types.h"
#include "print.h"

void clear() {
    char* vid = (char*)0xb8000;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i++) {
        vid[i * 2 + 0] = 0;
    }
}

int str_len(string str) {
    int len;
    for (len = 0; str[len] > 0; len++) { }
    return len;
}

void print(int offset, string str) {
    char* vid = (char*)0xb8000;

    for (int i = 0; str[i] > 0; i++) {
        vid[offset * 2 + i * 2 + 0] = str[i];
        vid[offset * 2 + i * 2 + 1] = 2;
    }
}

string num_to_str(int num, int r) {
    if (num == 0) return "0";

    string str = "0000000000000000";
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
    return (string)((ulong)str + (end - i));
}

string char_to_str(char c) {
    string str = " ";
    str[0] = c;
    return str;
}