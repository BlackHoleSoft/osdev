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


// Initialize module structure
void wasm_module_init(wasm_module_t *module) {
    module->magic = 0;
    module->version = 0;
    module->type_count = 0;
    module->types = NULL;
    module->import_count = 0;
    module->func_count = 0;
    module->func_types = NULL;
    module->export_count = 0;
    module->exports = NULL;
    module->code_count = 0;
    module->codes = NULL;
    module->start_func_index = UINT32_MAX;  // Invalid index by default
}

// Parse the type section
int wasm_parse_type_section(const uint8_t **data, const uint8_t *end, wasm_module_t *module) {
    uint32_t count = decode_u32leb128(data, end);
    if (*data >= end) return 0;
    
    module->type_count = count;
    if (count > 0) {
        module->types = (wasm_function_type_t*)malloc(sizeof(wasm_function_type_t) * count);
        if (!module->types) {
            println("WASM: Failed to allocate memory for types\n");
            return 0;
        }
        
        for (uint32_t i = 0; i < count; i++) {
            if (*data >= end) return 0;
            
            module->types[i].form = **data;
            (*data)++;
            
            if (module->types[i].form != WASM_FUNC) {
                println("WASM: Unsupported function form\n");
                return 0;
            }
            
            module->types[i].param_count = decode_u32leb128(data, end);
            if (*data >= end) return 0;
            
            if (module->types[i].param_count > 0) {
                module->types[i].param_types = (uint8_t*)malloc(module->types[i].param_count);
                if (!module->types[i].param_types) {
                    println("WASM: Failed to allocate memory for param types\n");
                    return 0;
                }
                
                for (uint32_t j = 0; j < module->types[i].param_count; j++) {
                    if (*data >= end) return 0;
                    module->types[i].param_types[j] = **data;
                    (*data)++;
                }
            } else {
                module->types[i].param_types = NULL;
            }
            
            module->types[i].result_count = decode_u32leb128(data, end);
            if (*data >= end) return 0;
            
            if (module->types[i].result_count > 0) {
                module->types[i].result_types = (uint8_t*)malloc(module->types[i].result_count);
                if (!module->types[i].result_types) {
                    println("WASM: Failed to allocate memory for result types\n");
                    return 0;
                }
                
                for (uint32_t j = 0; j < module->types[i].result_count; j++) {
                    if (*data >= end) return 0;
                    module->types[i].result_types[j] = **data;
                    (*data)++;
                }
            } else {
                module->types[i].result_types = NULL;
            }
        }
    }
    
    return 1;
}

// Parse the function section
int wasm_parse_function_section(const uint8_t **data, const uint8_t *end, wasm_module_t *module) {
    uint32_t count = decode_u32leb128(data, end);
    if (*data >= end) return 0;
    
    module->func_count = count;
    if (count > 0) {
        module->func_types = (uint32_t*)malloc(sizeof(uint32_t) * count);
        if (!module->func_types) {
            println("WASM: Failed to allocate memory for function types\n");
            return 0;
        }
        
        for (uint32_t i = 0; i < count; i++) {
            module->func_types[i] = decode_u32leb128(data, end);
            if (*data >= end) return 0;
        }
    }
    
    return 1;
}

// Parse the code section
int wasm_parse_code_section(const uint8_t **data, const uint8_t *end, wasm_module_t *module) {
    uint32_t count = decode_u32leb128(data, end);
    if (*data >= end) return 0;
    
    module->code_count = count;
    if (count > 0) {
        module->codes = (wasm_function_def_t*)malloc(sizeof(wasm_function_def_t) * count);
        if (!module->codes) {
            println("WASM: Failed to allocate memory for function codes\n");
            return 0;
        }
        
        for (uint32_t i = 0; i < count; i++) {
            // Get the size of the function body
            uint32_t body_size = decode_u32leb128(data, end);
            if (*data >= end) return 0;
            
            const uint8_t *body_start = *data;
            
            // Read local variable declarations (we'll skip them for now)
            uint32_t local_count = decode_u32leb128(data, end);
            if (*data >= end) return 0;
            
            for (uint32_t j = 0; j < local_count; j++) {
                // Skip local count
                decode_u32leb128(data, end);
                if (*data >= end) return 0;
                // Skip local type
                **data;
                (*data)++;
                if (*data >= end) return 0;
            }
            
            // Calculate the actual code size (excluding local declarations)
            module->codes[i].code_size = body_size - (*data - body_start);
            module->codes[i].code = *data;
            module->codes[i].type_index = (i < module->func_count) ? module->func_types[i] : 0;
            
            // Move to the end of the function body
            *data += module->codes[i].code_size;
            if (*data > end) return 0;
        }
    }
    
    return 1;
}

// Parse the start section
int wasm_parse_start_section(const uint8_t **data, const uint8_t *end, wasm_module_t *module) {
    module->start_func_index = decode_u32leb128(data, end);
    if (*data > end) return 0;
    return 1;
}

// Parse export section
int wasm_parse_export_section(const uint8_t **data, const uint8_t *end, wasm_module_t *module) {
    uint32_t count = decode_u32leb128(data, end);
    if (*data >= end) return 0;
    
    module->export_count = count;
    if (count > 0) {
        module->exports = (wasm_export_t*)malloc(sizeof(wasm_export_t) * count);
        if (!module->exports) {
            println("WASM: Failed to allocate memory for exports\n");
            return 0;
        }
        
        for (uint32_t i = 0; i < count; i++) {
            // Read the name of the export
            uint32_t name_len = decode_u32leb128(data, end);
            if (*data + name_len >= end) return 0;
            
            // Allocate memory for the name and copy it
            char *name = (char*)malloc(name_len + 1);
            if (!name) {
                println("WASM: Failed to allocate memory for export name\n");
                return 0;
            }
            
            for (uint32_t j = 0; j < name_len; j++) {
                name[j] = (*data)[j];
            }
            name[name_len] = '\0';
            (*data) += name_len;
            
            // Read the export kind (we only handle functions for now)
            uint8_t export_kind = **data;
            (*data)++;
            
            if (export_kind != 0x00) { // 0x00 is function export
                println("WASM: Unsupported export kind\n");
                free(name);
                return 0;
            }
            
            // Read the function index
            uint32_t func_index = decode_u32leb128(data, end);
            if (*data > end) {
                free(name);
                return 0;
            }
            
            // Store the export information
            module->exports[i].name = name;
            module->exports[i].index = func_index;
        }
    }
    
    return 1;
}

// Parse a single section
int wasm_parse_section(const uint8_t **data, const uint8_t *end, uint8_t section_id, wasm_module_t *module) {
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
            if (!wasm_parse_type_section(data, section_end, module)) {
                return 0;
            }
            break;
        case WASM_SECTION_IMPORT:
            println("WASM: Found import section\n");
            // Skip for now - we'll implement import parsing later
            *data = section_end;
            break;
        case WASM_SECTION_FUNCTION:
            println("WASM: Found function section\n");
            if (!wasm_parse_function_section(data, section_end, module)) {
                return 0;
            }
            break;
        case WASM_SECTION_TABLE:
            println("WASM: Found table section\n");
            // Skip for now
            *data = section_end;
            break;
        case WASM_SECTION_MEMORY:
            println("WASM: Found memory section\n");
            // Skip for now
            *data = section_end;
            break;
        case WASM_SECTION_GLOBAL:
            println("WASM: Found global section\n");
            // Skip for now
            *data = section_end;
            break;
        case WASM_SECTION_EXPORT:
            println("WASM: Found export section\n");
            if (!wasm_parse_export_section(data, section_end, module)) {
                return 0;
            }
            break;
        case WASM_SECTION_START:
            println("WASM: Found start section\n");
            if (!wasm_parse_start_section(data, section_end, module)) {
                return 0;
            }
            break;
        case WASM_SECTION_ELEMENT:
            println("WASM: Found element section\n");
            // Skip for now
            *data = section_end;
            break;
        case WASM_SECTION_CODE:
            println("WASM: Found code section\n");
            if (!wasm_parse_code_section(data, section_end, module)) {
                return 0;
            }
            break;
        case WASM_SECTION_DATA:
            println("WASM: Found data section\n");
            // Skip for now
            *data = section_end;
            break;
        case WASM_SECTION_CUSTOM:
            println("WASM: Found custom section\n");
            // Skip for now
            *data = section_end;
            break;
        default:
            println("WASM: Unknown section ID\n");
            *data = section_end;
            break;
    }
    
    // Ensure we're at the end of the section
    if (*data != section_end) {
        println("WASM: Section parsing error - unexpected position\n");
        return 0;
    }
    
    return 1;
}

// Parse the entire WASM module
int wasm_parse_module(wasm_module_t *module, const uint8_t *data, size_t size) {
    if (!wasm_validate_header(data, size)) {
        return 0;
    }
    
    // Initialize the module structure
    wasm_module_init(module);
    
    const uint8_t *ptr = data + 8;  // Skip header
    const uint8_t *end = data + size;
    
    // Set the magic and version
    module->magic = WASM_MAGIC;
    module->version = WASM_VERSION;
    
    // Parse sections
    while (ptr < end) {
        uint8_t section_id = *ptr;
        ptr++;
        
        if (!wasm_parse_section(&ptr, end, section_id, module)) {
            // Clean up allocated memory on error
            wasm_module_destroy(module);
            return 0;
        }
    }
    
    return 1;
}

// Destroy module and free memory
void wasm_module_destroy(wasm_module_t *module) {
    if (module->types) {
        for (uint32_t i = 0; i < module->type_count; i++) {
            if (module->types[i].param_types) {
                free(module->types[i].param_types);
            }
            if (module->types[i].result_types) {
                free(module->types[i].result_types);
            }
        }
        free(module->types);
        module->types = NULL;
    }
    
    if (module->func_types) {
        free(module->func_types);
        module->func_types = NULL;
    }
    
    if (module->codes) {
        free(module->codes);
        module->codes = NULL;
    }
    
    if (module->exports) {
        for (uint32_t i = 0; i < module->export_count; i++) {
            if (module->exports[i].name) {
                free((void*)module->exports[i].name);
            }
        }
        free(module->exports);
        module->exports = NULL;
    }
    
    module->type_count = 0;
    module->func_count = 0;
    module->export_count = 0;
    module->code_count = 0;
}
