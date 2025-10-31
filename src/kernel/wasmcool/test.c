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
    if (!wasm_vm_load_module(vm, test_add_module, sizeof(test_add_module))) {
        println("WASM: Failed to load test module\n");
        wasm_vm_destroy(vm);
        return;
    }
    
    println("WASM: Test module loaded successfully\n");
    
    
    
    // Clean up
    wasm_vm_destroy(vm);
    println("WASM: VM test completed\n");
}
