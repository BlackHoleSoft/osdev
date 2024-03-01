#include "string.h"
#include "mem.h"

int str_len(string str) {
    int len;
    for (len = 0; str[len] > 0; len++) { }
    return len;
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

string str_concat(string buffer, string s1, string s2) {
    int len = 0;
    int i;
    for (i=0; s1[i] > 0; i++) {
        buffer[i] = s1[i];
        len++;
        if (len >= 512) return buffer;
    }
    for (i=0; s2[i] > 0; i++) {
        buffer[len + i] = s2[i];
        if (len >= 512) return buffer;
    }
    buffer[len + i] = '\0';
    return buffer;
}

string str_copy(string buffer, string value) {
    int i;
    for (i=0; value[i] > 0 && i < 511; i++) {
        buffer[i] = value[i];
    }
    buffer[i] = '\0';
}