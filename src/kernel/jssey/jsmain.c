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



void sys_print(char* str) {    
    print_colored(str, 0x3);
}

double js_run(void* bytecode, struct JsseyState* state, int instructions_cnt, bool debug) {
    if (state->is_initialized == false) {
        state->memory = mem_10kb();
        state->variables = mem_10kb();
        state->stack = mem_10kb();
        state->call_stack = mem_10kb();
        state->var_types = mem_10kb();
        state->stack_types = mem_10kb();
        state->stack_ptr = -1;
        state->call_stack_pointer = -1;
        state->memory_pointer = 0;

        state->code = (struct ByteCode*)bytecode;
        state->fn_main_ptr = (char*)&state->code->functions;
        state->code_pointer = state->fn_main_ptr + 4;
        state->main_length = *(int*)state->fn_main_ptr;

        state->is_running = true;
        state->is_initialized = true;
    }    

    if (debug) print("Fn main length: ");
    if (debug) println(num_to_str(state->main_length, 10));
    
    int instr_counter = 0;
    while(state->is_running && instr_counter < instructions_cnt) {
        u8 value_u8 = state->code_pointer[0];

        if (debug) print("OPCODE: 0x");
        if (debug) print_colored(num_to_str(value_u8, 16), 0x4);
        if (debug) print("; CP: 0x");
        if (debug) print_colored(num_to_str((int)state->code_pointer, 16), 0x4);

        switch (value_u8)
        {
            case OPCODE_JMP:
                int local_addr = (int)*(double*)(state->code_pointer + 1);
                state->code_pointer = (char*)state->code + local_addr;
                break;

            case OPCODE_JMPIF:
                if ((int)state->stack[state->stack_ptr--]) {
                    int local_addr = (int)*(double*)(state->code_pointer + 1);
                    state->code_pointer = (char*)state->code + local_addr;
                } else {
                    state->code_pointer += 1 + 8 * 1;
                }               
                break;

            case OPCODE_JMPNOT:
                if ((int)state->stack[state->stack_ptr--] == 0) {
                    int local_addr = (int)*(double*)(state->code_pointer + 1);
                    state->code_pointer = (char*)state->code + local_addr;
                } else {
                    state->code_pointer += 1 + 8 * 1;
                }               
                break;
            
            case OPCODE_PROPGET:
                {
                    char* prop_name = state->memory + (int)state->stack[state->stack_ptr--];
                    char* obj = state->memory + (int)state->stack[state->stack_ptr--];
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
                            state->stack_types[state->stack_ptr + 1] = state->var_types[var_index];
                            state->stack[++state->stack_ptr] = state->variables[var_index];
                        }
                    }
                    state->code_pointer += 1;
                    break;
                }
            
            case OPCODE_PROPSET:
                {
                    double prop_new_value = state->stack[state->stack_ptr];
                    char prop_new_type = (char)state->stack_types[state->stack_ptr--];
                    char* prop_name = state->memory + (int)state->stack[state->stack_ptr--];
                    char* obj = state->memory + (int)state->stack[state->stack_ptr--];
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
                            state->var_types[var_index] = prop_new_type;
                            state->variables[var_index] = prop_new_value;
                        }
                    }
                    state->code_pointer += 1;
                    break;
                }

            case OPCODE_LOWER:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] > state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_GREATER:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] < state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_LE:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] >= state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_GE:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] <= state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_EQ:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] == state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_NOTEQ:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] != state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_PUSH:
                state->stack_types[state->stack_ptr + 1] = state->code_pointer[1];
                state->stack[++state->stack_ptr] = *(double*)(state->code_pointer + 2);
                state->code_pointer += 1 + 1 + 8 * 1;
                break;

            case OPCODE_POP:
                state->stack_ptr--;
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_VAR:
                state->var_types[(int)*(double*)(state->code_pointer + 1)] = state->stack_types[state->stack_ptr];
                state->variables[(int)*(double*)(state->code_pointer + 1)] = state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_GETVAR:
                state->stack_types[state->stack_ptr + 1] = state->var_types[(int)*(double*)(state->code_pointer + 1)];
                state->stack[++state->stack_ptr] = state->variables[(int)*(double*)(state->code_pointer + 1)];
                state->code_pointer += 1 + 8 * 1;
                break;

            case OPCODE_CALL: 
                int fnIndex = (int)*(double*)(state->code_pointer + 1);
                if (fnIndex < SYS_FNS_COUNT) {
                    char vtype = state->stack_types[state->stack_ptr];                    
                    switch (fnIndex)
                    {
                        case 1:
                            sys_print(vtype == VAR_TYPE_NUMBER || vtype == VAR_TYPE_BOOL ? 
                                num_to_str((int)state->stack[state->stack_ptr], 10) : state->memory + (int)(state->stack[state->stack_ptr]));
                            state->stack_ptr--;
                            break;
                    }
                    state->code_pointer += 1 + 8 * 1;
                    break;
                }

                state->call_stack[++state->call_stack_pointer] = state->code_pointer + 1 + 8 * 1;                
                char* fn_pointer = state->fn_main_ptr;
                for (int i=0; i<fnIndex; i++) {
                    fn_pointer = fn_pointer + 4 + *(int*)fn_pointer;
                }
                // jump to the function code
                state->code_pointer = fn_pointer + 4;
                if (debug) print("; CALL: ");
                if (debug) print(num_to_str((int)state->code_pointer, 16));
                if (debug) print(" ");
                break;

            case OPCODE_RET:
                if (state->call_stack_pointer < 0) {
                    state->is_running = false;
                    break;
                }
                state->code_pointer = state->call_stack[state->call_stack_pointer--];
                break;
            
            case OPCODE_MEM:
                int mlen = (int)*(double*)(state->code_pointer + 1);
                for (int i=0; i<mlen; i++) {
                    state->memory[state->memory_pointer + i] = *(state->code_pointer + 1 + 8 * 1 + i);
                }
                state->stack_types[state->stack_ptr + 1] = VAR_TYPE_POINTER;
                state->stack[++state->stack_ptr] = state->memory_pointer;
                state->memory_pointer += mlen;
                state->code_pointer += 1 + 8 * 1 + mlen;
                break;

            case OPCODE_ADD:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] + state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_SUB:
                state->stack[state->stack_ptr - 1] = state->stack[state->stack_ptr-1] - state->stack[state->stack_ptr];
                state->stack_ptr -= 1;
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_MUL:
                state->stack[++state->stack_ptr] = state->stack[state->stack_ptr--] * state->stack[state->stack_ptr--];
                state->code_pointer += 1 + 8 * 0;
                break;

            case OPCODE_DIV:
                state->stack[state->stack_ptr - 1] = state->stack[state->stack_ptr-1] / state->stack[state->stack_ptr];
                state->stack_ptr -= 1;
                state->code_pointer += 1 + 8 * 0;
                break;

            default:
                break;
        }

        if (debug) {
            print("; Stack:");
            for (int i = 0; i<4 && i <= state->stack_ptr; i++) {
                print_colored(num_to_str((int)state->stack[state->stack_ptr - i], 10), 0x4);
                print(", ");
            }
            println("");
        }
        
        instr_counter++;
    }

    if (debug) {
        print("Variables: ");
        for (int i = 0; i<state->code->variables_count; i++) {
            print(num_to_str(state->var_types[i], 16));
            print(":");
            print(num_to_str((int)state->variables[i], 10));
            print("; ");
        }
        println("");

        print("Stack: ");
        for (int i = 0; i<4; i++) {
            print(num_to_str(state->stack_types[state->stack_ptr - i], 16));
            print(":");
            print(num_to_str((int)state->stack[state->stack_ptr - i], 10));
            print("; ");
        }
        println("");

        print("Memory: ");
        for (int i = 0; i<80; i++) {
            print(num_to_str(state->memory[i], 16));
            print(" ");
        }
        println("");
    }

    if (state->is_running == false) {
        state->result = state->stack[state->stack_ptr];
    }

    return state->stack[state->stack_ptr];
}