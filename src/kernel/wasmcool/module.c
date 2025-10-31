#include "wasmcool.h"
#include "../std/print.h"
#include "../std/mem.h"
#include "../std/string.h"

// Function to validate the WASM header
int wasm_validate_header(const uint8_t *data, size_t size) {
    if (size < 8) {
        println("WASM: Module too small to contain header\n");
        return 0;
    }
    
    uint32_t magic = ((uint32_t)data[0]) | 
                     ((uint32_t)data[1] << 8) | 
                     ((uint32_t)data[2] << 16) | 
                     ((uint32_t)data[3] << 24);
    
    uint32_t version = ((uint32_t)data[4]) | 
                       ((uint32_t)data[5] << 8) | 
                       ((uint32_t)data[6] << 16) | 
                       ((uint32_t)data[7] << 24);
    
    if (magic != WASM_MAGIC) {
        println("WASM: Invalid magic number\n");
        return 0;
    }
    
    if (version != WASM_VERSION) {
        println("WASM: Unsupported version\n");
        return 0;
    }
    
    return 1;
}

// Parse a single section
int wasm_parse_section(const uint8_t **data, const uint8_t *end, uint8_t section_id) {
    // Get the size of the section
    uint32_t size = decode_u32leb128(data, end);
    if (*data + size > end) {
        println("WASM: Section size exceeds module bounds\n");
        return 0;
    }
    
    const uint8_t *section_start = *data;
    const uint8_t *section_end = *data + size;
    
    // Process section based on its ID
    switch (section_id) {
        case WASM_SECTION_TYPE:
            println("WASM: Found type section\n");
            // Skip for now - we'll implement type parsing later
            break;
        case WASM_SECTION_IMPORT:
            println("WASM: Found import section\n");
            // Skip for now
            break;
        case WASM_SECTION_FUNCTION:
            println("WASM: Found function section\n");
            // Skip for now
            break;
        case WASM_SECTION_TABLE:
            println("WASM: Found table section\n");
            // Skip for now
            break;
        case WASM_SECTION_MEMORY:
            println("WASM: Found memory section\n");
            // Skip for now
            break;
        case WASM_SECTION_GLOBAL:
            println("WASM: Found global section\n");
            // Skip for now
            break;
        case WASM_SECTION_EXPORT:
            println("WASM: Found export section\n");
            // Skip for now
            break;
        case WASM_SECTION_START:
            println("WASM: Found start section\n");
            // Skip for now
            break;
        case WASM_SECTION_ELEMENT:
            println("WASM: Found element section\n");
            // Skip for now
            break;
        case WASM_SECTION_CODE:
            println("WASM: Found code section\n");
            // Skip for now
            break;
        case WASM_SECTION_DATA:
            println("WASM: Found data section\n");
            // Skip for now
            break;
        case WASM_SECTION_CUSTOM:
            println("WASM: Found custom section\n");
            // Skip for now
            break;
        default:
            println("WASM: Unknown section ID\n");
            break;
    }
    
    // Move to the end of this section
    *data = section_end;
    
    return 1;
}

// Parse the entire WASM module
int wasm_parse_module(wasm_module_t *module, const uint8_t *data, size_t size) {
    if (!wasm_validate_header(data, size)) {
        return 0;
    }
    
    const uint8_t *ptr = data + 8;  // Skip header
    const uint8_t *end = data + size;
    
    // Set the magic and version
    module->magic = WASM_MAGIC;
    module->version = WASM_VERSION;
    
    // Parse sections
    while (ptr < end) {
        uint8_t section_id = *ptr;
        ptr++;
        
        if (!wasm_parse_section(&ptr, end, section_id)) {
            return 0;
        }
    }
    
    return 1;
}
