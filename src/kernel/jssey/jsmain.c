#include "jsmain.h"
#include "../std/string.h"
#include "../std/mem.h"
#include "../std/print.h"

#define OPCODE_ADD 0x20
#define OPCODE_SUB 0x21
#define OPCODE_MUL 0x22
#define OPCODE_DIV 0x23
#define OPCODE_SET 0x40

#define OPCODE_PROPGET 0x10
#define OPCODE_PROPSET 0x11

#define OPCODE_LOWER 0x50
#define OPCODE_GREATER 0x51
#define OPCODE_LE 0x52
#define OPCODE_GE 0x53
#define OPCODE_EQ 0x54
#define OPCODE_NOTEQ 0x55

#define OPCODE_PUSH 0xa
#define OPCODE_POP 0xb
#define OPCODE_STACKCPY 0xc

#define OPCODE_VAR 0x3
#define OPCODE_GETVAR 0x4
#define OPCODE_RET 0x5
#define OPCODE_CALL 0x6
#define OPCODE_MEM 0x7
#define OPCODE_MEMUPD 0x8

#define OPCODE_JMPNOT 0xA1
#define OPCODE_JMPIF 0xA2
#define OPCODE_JMP 0xA3

#define VAR_TYPE_NUMBER 0x0
#define VAR_TYPE_STRING 0x1
#define VAR_TYPE_BOOL 0x2
#define VAR_TYPE_OBJECT 0x3
#define VAR_TYPE_POINTER 0x4

#define PROP_NAME_LENGTH 16
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
    string memory = mem_10kb();
    double* variables = mem_10kb();
    double* stack = mem_10kb();
    char** call_stack = mem_10kb();
    char* var_types = mem_10kb();
    char* stack_types = mem_10kb();
    int stack_ptr = -1;
    int call_stack_pointer = -1;
    int memory_pointer = 0;

    struct ByteCode* code = (struct ByteCode*)bytecode;
    char* fn_main_ptr = (char*)&code->functions;
    char* code_pointer = fn_main_ptr + 4;
    int main_length = *(int*)fn_main_ptr;

    if (debug) print("Fn main length: ");
    if (debug) println(num_to_str(main_length, 10));
    
    bool running = true;
    while(running) {
        u8 value_u8 = code_pointer[0];

        if (debug) print("OPCODE: 0x");
        if (debug) print_colored(num_to_str(value_u8, 16), 0x4);
        if (debug) print("; CP: 0x");
        if (debug) print_colored(num_to_str((int)code_pointer, 16), 0x4);

        switch (value_u8)
        {
            case OPCODE_JMP:
                int local_addr = (int)*(double*)(code_pointer + 1);
                code_pointer = (char*)code + local_addr;
                break;

            case OPCODE_JMPIF:
                if ((int)stack[stack_ptr--]) {
                    int local_addr = (int)*(double*)(code_pointer + 1);
                    code_pointer = (char*)code + local_addr;
                } else {
                    code_pointer += 1 + 8 * 1;
                }               
                break;

            case OPCODE_JMPNOT:
                if ((int)stack[stack_ptr--] == 0) {
                    int local_addr = (int)*(double*)(code_pointer + 1);
                    code_pointer = (char*)code + local_addr;
                } else {
                    code_pointer += 1 + 8 * 1;
                }               
                break;
            
            case OPCODE_PROPGET:
                char* prop_name = memory + (int)stack[stack_ptr--];
                char* obj = memory + (int)stack[stack_ptr--];
                for (int i = 0; i < *(int*)obj; i++) {
                    int offset = 4 + i * (PROP_NAME_LENGTH + 1 + 4);
                    string pname = obj + offset;
                    bool eq = true;
                    for (int j=0; prop_name[j] > 0; j++) {
                        eq = eq && prop_name[j] == pname[j];
                    }
                    if (eq) {
                        // load var value to the stack
                        int var_index = *(int*)(obj + offset + PROP_NAME_LENGTH + 1);
                        stack_types[stack_ptr + 1] = var_types[var_index];
                        stack[++stack_ptr] = variables[var_index];
                    }
                }
                code_pointer += 1;
                break;

            case OPCODE_LOWER:
                stack[++stack_ptr] = stack[stack_ptr--] > stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_GREATER:
                stack[++stack_ptr] = stack[stack_ptr--] < stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_LE:
                stack[++stack_ptr] = stack[stack_ptr--] >= stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_GE:
                stack[++stack_ptr] = stack[stack_ptr--] <= stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_EQ:
                stack[++stack_ptr] = stack[stack_ptr--] == stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_NOTEQ:
                stack[++stack_ptr] = stack[stack_ptr--] != stack[stack_ptr--];
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_PUSH:
                stack_types[stack_ptr + 1] = code_pointer[1];
                stack[++stack_ptr] = *(double*)(code_pointer + 2);
                code_pointer += 1 + 1 + 8 * 1;
                break;

            case OPCODE_POP:
                stack_ptr--;
                code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_VAR:
                var_types[(int)*(double*)(code_pointer + 1)] = stack_types[stack_ptr];
                variables[(int)*(double*)(code_pointer + 1)] = stack[stack_ptr--];
                code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_GETVAR:
                stack_types[stack_ptr + 1] = var_types[(int)*(double*)(code_pointer + 1)];
                stack[++stack_ptr] = variables[(int)*(double*)(code_pointer + 1)];
                code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_CALL: 
                int fnIndex = (int)*(double*)(code_pointer + 1);
                if (fnIndex < SYS_FNS_COUNT) {
                    char vtype = stack_types[stack_ptr];                    
                    switch (fnIndex)
                    {
                        case 1:
                            sys_print(vtype == VAR_TYPE_NUMBER || vtype == VAR_TYPE_BOOL ? num_to_str((int)stack[stack_ptr], 10) : memory + (int)(stack[stack_ptr]));
                            stack_ptr--;
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
                stack_types[stack_ptr + 1] = VAR_TYPE_POINTER;
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
            print(num_to_str(var_types[i], 16));
            print(":");
            print(num_to_str((int)variables[i], 10));
            print("; ");
        }
        println("");

        print("Stack: ");
        for (int i = 0; i<4; i++) {
            print(num_to_str(stack_types[stack_ptr - i], 16));
            print(":");
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