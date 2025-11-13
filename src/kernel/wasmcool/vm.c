#include "wasmcool.h"
#include "../std/print.h"
#include "../std/newmem.h"
#include "../std/string.h"

// Initialize the VM stack
void vm_stack_init(vm_stack_t *stack) {
    stack->top = -1;
}

// Check if stack is empty
int vm_stack_is_empty(vm_stack_t *stack) {
    return stack->top == -1;
}

// Check if stack is full
int vm_stack_is_full(vm_stack_t *stack) {
    return stack->top == VM_STACK_SIZE - 1;
}

// Push a value onto the stack
int vm_stack_push(vm_stack_t *stack, int32_t value) {
    if (vm_stack_is_full(stack)) {
        println("VM: Stack overflow\n");
        return 0;
    }
    
    stack->data[++stack->top] = value;
    return 1;
}

// Pop a value from the stack
int32_t vm_stack_pop(vm_stack_t *stack) {
    if (vm_stack_is_empty(stack)) {
        println("VM: Stack underflow\n");
        return 0; // Return 0 in case of underflow
    }
    
    return stack->data[stack->top--];
}

// Create a new VM instance
wasm_vm_t* wasm_vm_create() {
    wasm_vm_t *vm = (wasm_vm_t*)malloc(sizeof(wasm_vm_t));
    if (!vm) {
        println("VM: Failed to allocate memory for VM\n");
        return NULL;
    }
    
    vm_stack_init(&vm->stack);
    vm->module = NULL;
    
    return vm;
}

// Destroy a VM instance
void wasm_vm_destroy(wasm_vm_t *vm) {
    if (vm) {
        if (vm->module) {
            free(vm->module);
        }
        free(vm);
    }
}

// Load a WASM module into the VM
int wasm_vm_load_module(wasm_vm_t *vm, const uint8_t *data, size_t size) {
    // Allocate memory for the module
    wasm_module_t *module = (wasm_module_t*)malloc(sizeof(wasm_module_t));
    if (!module) {
        println("VM: Failed to allocate memory for module\n");
        return 0;
    }
    
    // Parse the module
    if (!wasm_parse_module(module, data, size)) {
        free(module);
        println("VM: Failed to parse module\n");
        return 0;
    }
    
    // Store the module in the VM
    if (vm->module) {
        free(vm->module);
    }
    vm->module = module;
    
    return 1;
}

// Execute a single instruction
int vm_execute_instruction(wasm_vm_t *vm, const uint8_t **code_ptr, const uint8_t *code_end) {
    if (*code_ptr >= code_end) {
        return 0; // No more instructions
    }
    
    uint8_t opcode = **code_ptr;
    (*code_ptr)++;
    
    switch (opcode) {
        case WASM_OP_I32_CONST: {
            // Read a 32-bit signed LEB128 constant and push it to stack
            int32_t value = decode_i32leb128(code_ptr, code_end);
            if (!vm_stack_push(&vm->stack, value)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_I32_ADD: {
            // Pop two values, add them, and push result
            if (vm->stack.top < 1) {
                println("VM: Not enough operands for i32.add\n");
                return 0;
            }
            
            int32_t b = vm_stack_pop(&vm->stack);
            int32_t a = vm_stack_pop(&vm->stack);
            int32_t result = a + b;
            
            if (!vm_stack_push(&vm->stack, result)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_I32_SUB: {
            // Pop two values, subtract second from first, and push result
            if (vm->stack.top < 1) {
                println("VM: Not enough operands for i32.sub\n");
                return 0;
            }
            
            int32_t b = vm_stack_pop(&vm->stack);
            int32_t a = vm_stack_pop(&vm->stack);
            int32_t result = a - b;
            
            if (!vm_stack_push(&vm->stack, result)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_I32_MUL: {
            // Pop two values, multiply them, and push result
            if (vm->stack.top < 1) {
                println("VM: Not enough operands for i32.mul\n");
                return 0;
            }
            
            int32_t b = vm_stack_pop(&vm->stack);
            int32_t a = vm_stack_pop(&vm->stack);
            int32_t result = a * b;
            
            if (!vm_stack_push(&vm->stack, result)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_I32_DIV_S: {
            // Pop two values, divide first by second (signed), and push result
            if (vm->stack.top < 1) {
                println("VM: Not enough operands for i32.div_s\n");
                return 0;
            }
            
            int32_t b = vm_stack_pop(&vm->stack);
            int32_t a = vm_stack_pop(&vm->stack);
            
            if (b == 0) {
                println("VM: Division by zero error for i32.div_s\n");
                return 0;
            }
            
            if (a == INT32_MIN && b == -1) {
                println("VM: Integer overflow error for i32.div_s\n");
                return 0;
            }
            
            int32_t result = a / b;
            
            if (!vm_stack_push(&vm->stack, result)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_I32_DIV_U: {
            // Pop two values, divide first by second (unsigned), and push result
            if (vm->stack.top < 1) {
                println("VM: Not enough operands for i32.div_u\n");
                return 0;
            }
            
            int32_t b = vm_stack_pop(&vm->stack);
            int32_t a = vm_stack_pop(&vm->stack);
            
            if (b == 0) {
                println("VM: Division by zero error for i32.div_u\n");
                return 0;
            }
            
            // Cast to unsigned for unsigned division
            uint32_t ua = (uint32_t)a;
            uint32_t ub = (uint32_t)b;
            uint32_t result = ua / ub;
            
            if (!vm_stack_push(&vm->stack, (int32_t)result)) {
                return 0;
            }
            break;
        }
        
        case WASM_OP_DROP: {
            // Drop the top value from the stack
            if (vm_stack_is_empty(&vm->stack)) {
                println("VM: Cannot drop from empty stack\n");
                return 0;
            }
            vm_stack_pop(&vm->stack);
            break;
        }
        
        case WASM_OP_END: {
            // End of block/function, return success
            return 1;
        }
        
        default:
            println("VM: Unsupported opcode: ");
            print_int(opcode);
            println("\n");
            return 0;
    }
    
    return 1;
}

// Run the VM with specific code
int wasm_vm_execute_code(wasm_vm_t *vm, const uint8_t *code, size_t code_size) {
    const uint8_t *ptr = code;
    const uint8_t *end = code + code_size;
    
    while (ptr < end) {
        if (!vm_execute_instruction(vm, &ptr, end)) {
            return 0;
        }
    }
    
    return 1;
}

// Find function index by name
int wasm_find_function_by_name(wasm_module_t *module, const char *func_name) {
    if (!module || !func_name) {
        return -1;
    }
    
    // Look through the exports to find the function by name
    for (uint32_t i = 0; i < module->export_count; i++) {
        if (module->exports[i].name && str_compare(module->exports[i].name, func_name) == 0) {
            return module->exports[i].index;
        }
    }
    
    // If not found in exports, return -1
    return -1;
}

// Main run function for the VM
int wasm_vm_run_func_index(wasm_vm_t *vm, uint32_t func_index) {
    if (!vm || !vm->module) {
        println("VM: Invalid VM or module\n");
        return 0;
    }
    
    // Check if the function index is valid
    if (func_index >= vm->module->func_count) {
        println("VM: Function index out of bounds\n");
        return 0;
    }
    
    // Find the corresponding code for this function
    // The function index in the code section might be different from the function index in the function section
    // In a full implementation, we would need to map these properly
    uint32_t code_index = func_index;
    if (code_index >= vm->module->code_count) {
        println("VM: No code found for function\n");
        return 0;
    }
    
    // Get the function definition
    wasm_function_def_t *func_def = &vm->module->codes[code_index];
    if (!func_def->code) {
        println("VM: Function has no code\n");
        return 0;
    }
    
    println("VM: Running function index ");
    print_int(func_index);
    println("\n");
    
    // Execute the function code
    return wasm_vm_execute_code(vm, func_def->code, func_def->code_size);
}

// Run a function by name
int wasm_vm_run(wasm_vm_t *vm, const char *func_name) {
    if (!vm || !vm->module || !func_name) {
        println("VM: Invalid VM, module or function name\n");
        return 0;
    }
    
    // Find the function index by name
    int func_index = wasm_find_function_by_name(vm->module, func_name);
    if (func_index < 0) {
        println("VM: Function not found: ");
        println(func_name);
        println("\n");
        return 0;
    }
    
    // Run the function by its index
    return wasm_vm_run_func_index(vm, (uint32_t)func_index);
}
