#include "err.h"

void error(u16 code, string message) {
    print_colored("ERR#", 0x6);
    print_colored(num_to_str(code, 16), 0x6);
    print(" - ");
    println(message);
}