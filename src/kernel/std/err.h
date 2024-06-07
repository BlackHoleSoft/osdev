#include "types.h"
#include "print.h"

#define ERR_WASM_IMPORT_SIZE 0x100
#define ERR_WASM_CODE_SIZE 0x101
#define ERR_WASM_WRONG_MODULE 0x102
#define ERR_WASM_EXPORT_SIZE 0x103
#define ERR_WASM_TYPES_OVERFLOW 0x104

void error(u16 code, string message);