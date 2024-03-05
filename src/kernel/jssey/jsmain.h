#include "../std/types.h"

struct ByteCode {
    u16 variables_count;
    u16 functions_count;
    int functions;
};

struct JsseyState {
    bool is_initialized;
    bool is_running;
    bool is_finished;
    double result;
    string memory;
    double* variables;
    double* stack;
    char** call_stack;
    char* var_types;
    char* stack_types;
    int stack_ptr;
    int call_stack_pointer;
    int memory_pointer;
    struct ByteCode* code;
    char* fn_main_ptr;
    char* code_pointer;
    int main_length;
};

double js_run(void* bytecode, struct JsseyState* state, int instructions_cnt, bool debug);