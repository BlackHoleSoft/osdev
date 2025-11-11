#include "wasmcool.h"
#include "../std/print.h"


// Test function to run the WASM VM
void wasm_test(uint8_t *data) {
    println("WASM: Starting VM test\n");
    
    // Create a VM instance
    wasm_vm_t *vm = wasm_vm_create();
    if (!vm) {
        println("WASM: Failed to create VM\n");
        return;
    }
    
    println("WASM: VM created successfully\n");
    
    // Load the test module
    if (!wasm_vm_load_module(vm, data, 50)) {
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
