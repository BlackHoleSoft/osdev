#include "wasmcool.h"
#include "../std/print.h"

// Simple WASM module that exports a function to add two numbers
// This is the binary representation of a minimal WASM module that adds two i32 values
static const uint8_t test_add_module[] = {
    // WASM header: magic number and version
    0x00, 0x61, 0x73, 0x6D,  // Magic number \0asm
    0x01, 0x00, 0x00, 0x00,  // Version 1
    
    // Type section (1 function type: (i32, i32) -> i32)
    0x01, 0x07,             // Section ID and size
    0x01,                   // Count of types
    0x60,                   // Function type
    0x02,                   // 2 parameters
    0x7F, 0x7F,             // Both parameters are i32
    0x01,                   // 1 result
    0x7F,                   // Result is i32
    
    // Function section (1 function using type 0)
    0x03, 0x02,             // Section ID and size
    0x01,                   // Count of functions
    0x00,                   // Function signature index (type 0)
    
    // Code section (1 function body)
    0x0A, 0x09,             // Section ID and size
    0x01,                   // Count of function bodies
    
    // Function body 0
    0x07,                   // Body size
    0x00,                   // Local count (no locals)
    
    // Instructions:
    0x20, 0x00,             // local.get 0 (first parameter)
    0x20, 0x01,             // local.get 1 (second parameter)
    0x6A,                   // i32.add
    0x0B                    // end
};

// A simple WASM module with a main function that pushes 42 to the stack
static const uint8_t test_main_module[] = {
    // WASM header: magic number and version
    0x00, 0x61, 0x73, 0x6D,  // Magic number \0asm
    0x01, 0x00, 0x00, 0x00,  // Version 1
    
    // Type section (1 function type: () -> i32)
    0x01, 0x05,             // Section ID and size
    0x01,                   // Count of types
    0x60,                   // Function type
    0x0,                   // 0 parameters
    0x01,                   // 1 result
    0x7F,                   // Result is i32
    
    // Function section (1 function using type 0)
    0x03, 0x02,             // Section ID and size
    0x01,                   // Count of functions
    0x00,                   // Function signature index (type 0)
    
    // Code section (1 function body)
    0x0A, 0x06,             // Section ID and size
    0x01,                   // Count of function bodies
    
    // Function body 0 (main function)
    0x04,                   // Body size
    0x0,                   // Local count (no locals)
    0x41, 0x2A,             // i32.const 42
    0x0B                    // end
};

// Test function to run the WASM VM
void wasm_test() {
    println("WASM: Starting VM test\n");
    
    // Create a VM instance
    wasm_vm_t *vm = wasm_vm_create();
    if (!vm) {
        println("WASM: Failed to create VM\n");
        return;
    }
    
    println("WASM: VM created successfully\n");
    
    // Load the test module
    if (!wasm_vm_load_module(vm, test_main_module, sizeof(test_main_module))) {
        println("WASM: Failed to load test module\n");
        wasm_vm_destroy(vm);
        return;
    }
    
    println("WASM: Test module loaded successfully\n");
    
    // Run the first function (main function) which should be at index 0
    if (!wasm_vm_run_func_index(vm, 0)) {
        println("WASM: Failed to run function\n");
        wasm_vm_destroy(vm);
        return;
    }
    
    println("WASM: Function executed successfully\n");
    
    // Check the result on the stack
    if (!vm_stack_is_empty(&vm->stack)) {
        int32_t result = vm_stack_pop(&vm->stack);
        println("WASM: Function result: ");
        print_int(result);
        println("\n");
    } else {
        println("WASM: No result on stack\n");
    }
    
    // Clean up
    wasm_vm_destroy(vm);
    println("WASM: VM test completed\n");
}
