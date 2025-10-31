# WASM Cool - WebAssembly Virtual Machine for Strelka OS

WASM Cool is a lightweight WebAssembly virtual machine implementation for the Strelka operating system. It provides a secure and efficient execution environment for WebAssembly modules within the kernel.

## Features

- Parses WebAssembly binary format (v1.0)
- Implements core WebAssembly instructions
- Stack-based virtual machine architecture
- LEB128 encoding/decoding support
- Basic arithmetic and logical operations
- Memory-safe execution environment

## Architecture

The VM is organized into several components:

- `wasmcool.h` - Main header with definitions and API
- `leb128.c` - LEB128 encoding/decoding functions
- `module.c` - WASM module parsing functionality
- `vm.c` - Core virtual machine implementation
- `test.c` - Test functionality and examples

## Supported Instructions

Currently supports basic stack operations and arithmetic:
- Constants (i32.const)
- Arithmetic (i32.add, i32.sub, i32.mul)
- Stack operations (drop)
- Control flow (end)

## Usage

The VM can be used by calling:
1. `wasm_vm_create()` - Create a new VM instance
2. `wasm_vm_load_module()` - Load a WASM module
3. `wasm_vm_run_func_index()` - Execute a specific function
4. `wasm_vm_destroy()` - Clean up the VM instance

## Memory Management

The VM uses the kernel's memory management system through `newmem.h` for all allocations, ensuring compatibility with the Strelka OS memory model.
