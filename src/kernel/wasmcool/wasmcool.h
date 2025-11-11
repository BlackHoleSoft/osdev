#ifndef WASMCOOL_H
#define WASMCOOL_H

#include "../std/types.h"
#include "../std/newmem.h"
#include "../std/print.h"

// WebAssembly constants
#define WASM_MAGIC 0x6d736100  // "\0asm"
#define WASM_VERSION 0x01

// Type definitions using kernel types
typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

// Define constants
#define UINT32_MAX 0xFFFFFFFF
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF
#define INT32_MIN -2147483648

// Data types
typedef enum {
    WASM_I32 = 0x7f,
    WASM_I64 = 0x7e,
    WASM_F32 = 0x7d,
    WASM_F64 = 0x7c,
    WASM_ANYFUNC = 0x70,
    WASM_FUNC = 0x60,
    WASM_EMPTY = 0x40
} wasm_valtype;

typedef enum {
    WASM_SECTION_CUSTOM = 0,
    WASM_SECTION_TYPE = 1,
    WASM_SECTION_IMPORT = 2,
    WASM_SECTION_FUNCTION = 3,
    WASM_SECTION_TABLE = 4,
    WASM_SECTION_MEMORY = 5,
    WASM_SECTION_GLOBAL = 6,
    WASM_SECTION_EXPORT = 7,
    WASM_SECTION_START = 8,
    WASM_SECTION_ELEMENT = 9,
    WASM_SECTION_CODE = 10,
    WASM_SECTION_DATA = 11
} wasm_section_id;

// Opcodes
typedef enum {
    WASM_OP_UNREACHABLE = 0x00,
    WASM_OP_NOP = 0x01,
    WASM_OP_BLOCK = 0x02,
    WASM_OP_LOOP = 0x03,
    WASM_OP_IF = 0x04,
    WASM_OP_ELSE = 0x05,
    WASM_OP_END = 0x0b,
    WASM_OP_BR = 0x0c,
    WASM_OP_BR_IF = 0x0d,
    WASM_OP_BR_TABLE = 0x0e,
    WASM_OP_RETURN = 0x0f,
    WASM_OP_CALL = 0x10,
    WASM_OP_CALL_INDIRECT = 0x11,
    WASM_OP_DROP = 0x1a,
    WASM_OP_SELECT = 0x1b,
    WASM_OP_LOCAL_GET = 0x20,
    WASM_OP_LOCAL_SET = 0x21,
    WASM_OP_LOCAL_TEE = 0x22,
    WASM_OP_GLOBAL_GET = 0x23,
    WASM_OP_GLOBAL_SET = 0x24,
    WASM_OP_I32_LOAD = 0x28,
    WASM_OP_I64_LOAD = 0x29,
    WASM_OP_F32_LOAD = 0x2a,
    WASM_OP_F64_LOAD = 0x2b,
    WASM_OP_I32_LOAD8_S = 0x2c,
    WASM_OP_I32_LOAD8_U = 0x2d,
    WASM_OP_I32_LOAD16_S = 0x2e,
    WASM_OP_I32_LOAD16_U = 0x2f,
    WASM_OP_I64_LOAD8_S = 0x30,
    WASM_OP_I64_LOAD8_U = 0x31,
    WASM_OP_I64_LOAD16_S = 0x32,
    WASM_OP_I64_LOAD16_U = 0x33,
    WASM_OP_I64_LOAD32_S = 0x34,
    WASM_OP_I64_LOAD32_U = 0x35,
    WASM_OP_I32_STORE = 0x36,
    WASM_OP_I64_STORE = 0x37,
    WASM_OP_F32_STORE = 0x38,
    WASM_OP_F64_STORE = 0x39,
    WASM_OP_I32_STORE8 = 0x3a,
    WASM_OP_I32_STORE16 = 0x3b,
    WASM_OP_I64_STORE8 = 0x3c,
    WASM_OP_I64_STORE16 = 0x3d,
    WASM_OP_I64_STORE32 = 0x3e,
    WASM_OP_MEMORY_SIZE = 0x3f,
    WASM_OP_MEMORY_GROW = 0x40,
    WASM_OP_I32_CONST = 0x41,
    WASM_OP_I64_CONST = 0x42,
    WASM_OP_F32_CONST = 0x43,
    WASM_OP_F64_CONST = 0x44,
    WASM_OP_I32_EQZ = 0x45,
    WASM_OP_I32_EQ = 0x46,
    WASM_OP_I32_NE = 0x47,
    WASM_OP_I32_LT_S = 0x48,
    WASM_OP_I32_LT_U = 0x49,
    WASM_OP_I32_GT_S = 0x4a,
    WASM_OP_I32_GT_U = 0x4b,
    WASM_OP_I32_LE_S = 0x4c,
    WASM_OP_I32_LE_U = 0x4d,
    WASM_OP_I32_GE_S = 0x4e,
    WASM_OP_I32_GE_U = 0x4f,
    WASM_OP_I64_EQZ = 0x50,
    WASM_OP_I64_EQ = 0x51,
    WASM_OP_I64_NE = 0x52,
    WASM_OP_I64_LT_S = 0x53,
    WASM_OP_I64_LT_U = 0x54,
    WASM_OP_I64_GT_S = 0x55,
    WASM_OP_I64_GT_U = 0x56,
    WASM_OP_I64_LE_S = 0x57,
    WASM_OP_I64_LE_U = 0x58,
    WASM_OP_I64_GE_S = 0x59,
    WASM_OP_I64_GE_U = 0x5a,
    WASM_OP_F32_EQ = 0x5b,
    WASM_OP_F32_NE = 0x5c,
    WASM_OP_F32_LT = 0x5d,
    WASM_OP_F32_GT = 0x5e,
    WASM_OP_F32_LE = 0x5f,
    WASM_OP_F32_GE = 0x60,
    WASM_OP_F64_EQ = 0x61,
    WASM_OP_F64_NE = 0x62,
    WASM_OP_F64_LT = 0x63,
    WASM_OP_F64_GT = 0x64,
    WASM_OP_F64_LE = 0x65,
    WASM_OP_F64_GE = 0x66,
    WASM_OP_I32_CLZ = 0x67,
    WASM_OP_I32_CTZ = 0x68,
    WASM_OP_I32_POPCNT = 0x69,
    WASM_OP_I32_ADD = 0x6a,
    WASM_OP_I32_SUB = 0x6b,
    WASM_OP_I32_MUL = 0x6c,
    WASM_OP_I32_DIV_S = 0x6d,
    WASM_OP_I32_DIV_U = 0x6e,
    WASM_OP_I32_REM_S = 0x6f,
    WASM_OP_I32_REM_U = 0x70,
    WASM_OP_I32_AND = 0x71,
    WASM_OP_I32_OR = 0x72,
    WASM_OP_I32_XOR = 0x73,
    WASM_OP_I32_SHL = 0x74,
    WASM_OP_I32_SHR_S = 0x75,
    WASM_OP_I32_SHR_U = 0x76,
    WASM_OP_I32_ROTL = 0x77,
    WASM_OP_I32_ROTR = 0x78,
    WASM_OP_I64_CLZ = 0x79,
    WASM_OP_I64_CTZ = 0x7a,
    WASM_OP_I64_POPCNT = 0x7b,
    WASM_OP_I64_ADD = 0x7c,
    WASM_OP_I64_SUB = 0x7d,
    WASM_OP_I64_MUL = 0x7e,
    WASM_OP_I64_DIV_S = 0x7f,
    WASM_OP_I64_DIV_U = 0x80,
    WASM_OP_I64_REM_S = 0x81,
    WASM_OP_I64_REM_U = 0x82,
    WASM_OP_I64_AND = 0x83,
    WASM_OP_I64_OR = 0x84,
    WASM_OP_I64_XOR = 0x85,
    WASM_OP_I64_SHL = 0x86,
    WASM_OP_I64_SHR_S = 0x87,
    WASM_OP_I64_SHR_U = 0x88,
    WASM_OP_I64_ROTL = 0x89,
    WASM_OP_I64_ROTR = 0x8a,
    WASM_OP_F32_ABS = 0x8b,
    WASM_OP_F32_NEG = 0x8c,
    WASM_OP_F32_CEIL = 0x8d,
    WASM_OP_F32_FLOOR = 0x8e,
    WASM_OP_F32_TRUNC = 0x8f,
    WASM_OP_F32_NEAREST = 0x90,
    WASM_OP_F32_SQRT = 0x91,
    WASM_OP_F32_ADD = 0x92,
    WASM_OP_F32_SUB = 0x93,
    WASM_OP_F32_MUL = 0x94,
    WASM_OP_F32_DIV = 0x95,
    WASM_OP_F32_MIN = 0x96,
    WASM_OP_F32_MAX = 0x97,
    WASM_OP_F32_COPYSIGN = 0x98,
    WASM_OP_F64_ABS = 0x99,
    WASM_OP_F64_NEG = 0x9a,
    WASM_OP_F64_CEIL = 0x9b,
    WASM_OP_F64_FLOOR = 0x9c,
    WASM_OP_F64_TRUNC = 0x9d,
    WASM_OP_F64_NEAREST = 0x9e,
    WASM_OP_F64_SQRT = 0x9f,
    WASM_OP_F64_ADD = 0xa0,
    WASM_OP_F64_SUB = 0xa1,
    WASM_OP_F64_MUL = 0xa2,
    WASM_OP_F64_DIV = 0xa3,
    WASM_OP_F64_MIN = 0xa4,
    WASM_OP_F64_MAX = 0xa5,
    WASM_OP_F64_COPYSIGN = 0xa6,
    WASM_OP_I32_WRAP_I64 = 0xa7,
    WASM_OP_I32_TRUNC_F32_S = 0xa8,
    WASM_OP_I32_TRUNC_F32_U = 0xa9,
    WASM_OP_I32_TRUNC_F64_S = 0xaa,
    WASM_OP_I32_TRUNC_F64_U = 0xab,
    WASM_OP_I64_EXTEND_I32_S = 0xac,
    WASM_OP_I64_EXTEND_I32_U = 0xad,
    WASM_OP_I64_TRUNC_F32_S = 0xae,
    WASM_OP_I64_TRUNC_F32_U = 0xaf,
    WASM_OP_I64_TRUNC_F64_S = 0xb0,
    WASM_OP_I64_TRUNC_F64_U = 0xb1,
    WASM_OP_F32_CONVERT_I32_S = 0xb2,
    WASM_OP_F32_CONVERT_I32_U = 0xb3,
    WASM_OP_F32_CONVERT_I64_S = 0xb4,
    WASM_OP_F32_CONVERT_I64_U = 0xb5,
    WASM_OP_F32_DEMOTE_F64 = 0xb6,
    WASM_OP_F64_CONVERT_I32_S = 0xb7,
    WASM_OP_F64_CONVERT_I32_U = 0xb8,
    WASM_OP_F64_CONVERT_I64_S = 0xb9,
    WASM_OP_F64_CONVERT_I64_U = 0xba,
    WASM_OP_F64_PROMOTE_F32 = 0xbb,
    WASM_OP_I32_REINTERPRET_F32 = 0xbc,
    WASM_OP_I64_REINTERPRET_F64 = 0xbd,
    WASM_OP_F32_REINTERPRET_I32 = 0xbe,
    WASM_OP_F64_REINTERPRET_I64 = 0xbf
} wasm_opcode;

// LEB128 decoding functions
uint32_t decode_u32leb128(const uint8_t **data, const uint8_t *end);
int32_t decode_i32leb128(const uint8_t **data, const uint8_t *end);
uint64_t decode_u64leb128(const uint8_t **data, const uint8_t *end);
int64_t decode_i64leb128(const uint8_t **data, const uint8_t *end);

// Function type structure
typedef struct {
    uint8_t form;           // Should be WASM_FUNC (0x60)
    uint32_t param_count;
    uint8_t *param_types;
    uint32_t result_count;
    uint8_t *result_types;
} wasm_function_type_t;

// Function definition structure
typedef struct {
    uint32_t type_index;
    uint32_t code_size;
    const uint8_t *code;
} wasm_function_def_t;

// Export structure for function exports
typedef struct {
    uint32_t index;
    const char *name;
} wasm_export_t;

// WASM module structure
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t type_count;
    wasm_function_type_t *types;
    uint32_t import_count;
    uint32_t func_count;
    uint32_t *func_types;   // Maps function index to type index
    uint32_t export_count;
    wasm_export_t *exports; // Array of exported functions
    uint32_t code_count;
    wasm_function_def_t *codes;  // Function code definitions
    uint32_t start_func_index;   // Optional start function index
    // Sections will be parsed and stored here
} wasm_module_t;

// VM stack structure
#define VM_STACK_SIZE 1024
typedef struct {
    int32_t data[VM_STACK_SIZE];
    int32_t top;
} vm_stack_t;

// Virtual machine structure
typedef struct {
    vm_stack_t stack;
    wasm_module_t *module;
    // Additional VM state will be added here
} wasm_vm_t;

// Main VM functions
wasm_vm_t* wasm_vm_create();
void wasm_vm_destroy(wasm_vm_t *vm);
int wasm_vm_load_module(wasm_vm_t *vm, const uint8_t *data, size_t size);
int wasm_vm_run(wasm_vm_t *vm, const char *func_name);
int wasm_vm_run_func_index(wasm_vm_t *vm, uint32_t func_index);

// Stack operations
int vm_stack_push(vm_stack_t *stack, int32_t value);
int32_t vm_stack_pop(vm_stack_t *stack);
int vm_stack_is_empty(vm_stack_t *stack);
int vm_stack_is_full(vm_stack_t *stack);

// Test function
void wasm_test(uint8_t *data);

#endif // WASMCOOL_H
