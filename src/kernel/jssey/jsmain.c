#include "jsmain.h"
#include "../std/string.h"
#include "../std/mem.h"

// split into tokens ( 3 "let" 4 "test" 1 "=" 2 "35" )
string split_line(string buffer, string code) {
    int splits_len = 1;
    int prev_split_pos = 0;
    int buffer_ptr = 0;
    char splits[] = { ' ' };
    int i;
    for (i=0; code[i] > 0; i++) {
        bool do_split = false;
        for (int s=0; s<splits_len; s++) {
            do_split = do_split || splits[s] == code[i];
        }
        // create token
        if (do_split) {
            // substring ( 3 "let" 4 "test" 1 "=" 2 "35" )
            for (int j=0; j < i - prev_split_pos; j++) {
                buffer[buffer_ptr + j + 0] = i - prev_split_pos;    // length of token
                buffer[buffer_ptr + j + 1] = code[prev_split_pos + j + 1];  // token string
            }

            buffer_ptr += i - prev_split_pos;
            prev_split_pos = i;            
        }
    }
    buffer[i] = '\0';
    return buffer;
}

// gets priority array of splitted line
void get_priority(string buffer, int result[], string operations[], int priorities[], int opcount) {
    int pos = 0;
    int current = 0;
    while (buffer[pos] > 0) {
        int len = buffer[pos];
        string oper = &buffer[pos + 1];
        for (int i=0; i<opcount; i++) {
            bool compare = true;
            for (int j=0; j<len; j++) {
                compare = compare && oper[j] == operations[i][j];
            }
            // if oper in operations, then add priority to result at current op index
            if (compare) {
                result[current] = priorities[i];
                current++;
                break;
            }
        }
        

        pos++;
    }
}

void jsmain() {
    string test_code = "let test = 6 + 4";

    string line_splitted = split_line(mem_512(), test_code);
}