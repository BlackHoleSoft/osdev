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

#define SYS_FNS_COUNT 2     // main, etc...

struct ByteCode {
    u16 variables_count;
    u16 functions_count;
    int functions;
};

void sys_print(char* str) {
    print_colored(str, 0x3);
    println("");
}

double js_run(void* bytecode, bool debug) {
    string memory = mem_512();
    double* variables = mem_512();
    double* stack = mem_512();
    char** call_stack = mem_512();
    int stack_ptr = -1;
    int call_stack_pointer = -1;
    int memory_pointer;

    struct ByteCode* code = (struct ByteCode*)bytecode;
    char* fn_main_ptr = (char*)&code->functions;
    char* code_pointer = fn_main_ptr + 4;
    int main_length = *(int*)fn_main_ptr;

    if (debug) print("Fn main length: ");
    if (debug) println(num_to_str(main_length, 10));
    
    bool running = true;
    while(running) {
        char value_u8 = code_pointer[0];

        if (debug) print("OPCODE: 0x");
        if (debug) print_colored(num_to_str(value_u8, 16), 0x4);
        if (debug) print("; CP: 0x");
        if (debug) print_colored(num_to_str((int)code_pointer, 16), 0x4);

        switch (value_u8)
        {
            case OPCODE_PUSH:
                stack[++stack_ptr] = *(double*)(code_pointer + 1);
                code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_POP:
                stack_ptr--;
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_VAR:
                variables[(int)*(double*)(code_pointer + 1)] = stack[stack_ptr--];
                code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_GETVAR:
                stack[++stack_ptr] = variables[(int)*(double*)(code_pointer + 1)];
                code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_CALL: 
                int fnIndex = (int)*(double*)(code_pointer + 1);
                if (fnIndex < SYS_FNS_COUNT) {
                    switch (fnIndex)
                    {
                        case 1:
                            sys_print(memory + (int)(stack[stack_ptr--]));
                            break;
                    }
                    code_pointer += 1 + 8 * 1;
                    break;
                }

                call_stack[++call_stack_pointer] = code_pointer + 1 + 8 * 1;                
                char* fn_pointer = fn_main_ptr;
                for (int i=0; i<fnIndex; i++) {
                    fn_pointer = fn_pointer + 4 + *(int*)fn_pointer;
                }
                // jump to the function code
                code_pointer = fn_pointer + 4;
                if (debug) print("; CALL: ");
                if (debug) print(num_to_str((int)code_pointer, 16));
                if (debug) print(" ");
                break;

            case OPCODE_RET:
                if (call_stack_pointer < 0) {
                    running = false;
                    break;
                }
                code_pointer = call_stack[call_stack_pointer--];
                break;
            
            case OPCODE_MEM:
                int mlen = (int)*(double*)(code_pointer + 1);
                for (int i=0; i<mlen; i++) {
                    memory[memory_pointer + i] = *(code_pointer + 1 + 8 * 1 + i);
                }
                stack[++stack_ptr] = memory_pointer;
                memory_pointer += mlen;
                code_pointer += 1 + 8 * 1 + mlen;
                break;

            case OPCODE_ADD:
                stack[++stack_ptr] = stack[stack_ptr--] + stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_SUB:
                stack[stack_ptr - 1] = stack[stack_ptr-1] - stack[stack_ptr];
                stack_ptr -= 1;
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_MUL:
                stack[++stack_ptr] = stack[stack_ptr--] * stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_DIV:
                stack[stack_ptr - 1] = stack[stack_ptr-1] / stack[stack_ptr];
                stack_ptr -= 1;
                code_pointer += 1 + 8 * 0;
                break;

            default:
                break;
        }

        if (debug) {
            print("; Stack:");
            for (int i = 0; i<4 && i <= stack_ptr; i++) {
                print_colored(num_to_str((int)stack[stack_ptr - i], 10), 0x4);
                print(", ");
            }
            println("");
        }
        
    }

    if (debug) {
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

        print("Memory: ");
        for (int i = 0; i<80; i++) {
            print(num_to_str(memory[i], 16));
            print(" ");
        }
        println("");
    }

    return stack[stack_ptr];
}