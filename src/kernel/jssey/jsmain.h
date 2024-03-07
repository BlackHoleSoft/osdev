#include "../std/types.h"

#define VAR_TYPE_NUMBER 0x0
#define VAR_TYPE_STRING 0x1
#define VAR_TYPE_BOOL 0x2
#define VAR_TYPE_OBJECT 0x3
#define VAR_TYPE_POINTER 0x4

struct MemoryBlock {
    void* ptr;
    bool used;
};

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
    int error;
    string memory;
    bool* used_memory;
    double* variables;
    double* stack;
    char** call_stack;
    char* var_types;
    char* stack_types;
    int stack_ptr;
    int call_stack_pointer;
    int memory_pointer;
    int memory_size;
    int vars_count;
    struct ByteCode* code;
    char* fn_main_ptr;
    char* code_pointer;
    int main_length;
};

void js_propset(struct JsseyState* state, string prop_name, char* obj, double value, char type);
void js_propget(struct JsseyState* state, string prop_name, char* obj, double* out_value, u8* out_type);
double js_run(void* bytecode, struct JsseyState* state, int instructions_cnt, bool debug);