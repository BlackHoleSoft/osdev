#include "jsmain.h"
#include "../std/string.h"
#include "../std/mem.h"
#include "../std/print.h"

#define OPCODE_ADD 0x20
#define OPCODE_SUB 0x21
#define OPCODE_MUL 0x22
#define OPCODE_DIV 0x23
#define OPCODE_SET 0x40

#define OPCODE_PUSH 0xa
#define OPCODE_POP 0xb

#define OPCODE_VAR 0x3
#define OPCODE_GETVAR 0x4
#define OPCODE_RET 0x5
#define OPCODE_CALL 0x6
#define OPCODE_MEM 0x7

struct ByteCode {
    u16 variables_count;
    u16 functions_count;
    int functions;
};

void js_run(void* bytecode) {
    string memory = mem_512();
    double* variables = mem_512();
    double* stack = mem_512();
    int stack_ptr = -1;

    struct ByteCode* code = (struct ByteCode*)bytecode;
    char* fn_ptr = (char*)&code->functions;
    while (*(int*)fn_ptr > 0) {
        println(num_to_str(*(int*)fn_ptr, 10));
        int length = *(int*)fn_ptr;
        char* bytes = fn_ptr + 4;
        for (int cp=0; cp<length; ) {
            char value_u8 = bytes[cp];
            switch (value_u8)
            {
                case OPCODE_PUSH:
                    stack[++stack_ptr] = *(double*)(bytes + cp + 1);
                    cp += 1 + 8 * 1;
                    break;

                case OPCODE_POP:
                    stack_ptr--;
                    cp += 1 + 8 * 0;
                    break;

                case OPCODE_VAR:
                    variables[(int)*(double*)(bytes + cp + 1)] = stack[stack_ptr];
                    cp += 1 + 8 * 1;
                    break;

                case OPCODE_GETVAR:
                    stack[++stack_ptr] = variables[(int)*(double*)(bytes + cp + 1)];
                    cp += 1 + 8 * 1;
                    break;

                case OPCODE_ADD:
                    stack[++stack_ptr] = stack[stack_ptr--] + stack[stack_ptr--];
                    cp += 1 + 8 * 2;
                    break;

                default:
                    break;
            }
        }

        fn_ptr += length + 4;
    }

    print("Variables: ");
    for (int i = 0; i<code->variables_count; i++) {
        print(num_to_str((int)variables[i], 10));
        print("; ");
    }
    println("");

    print("Stack: ");
    for (int i = 0; i<4; i++) {
        print(num_to_str((int)stack[stack_ptr - i], 10));
        print("; ");
    }
    println("");
}