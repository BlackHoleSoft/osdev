#include "err.h"

void error(u16 code, string message) {
    print_colored("ERROR: ", 0x6);
    print(num_to_str(code, 16));
    println(message);
}