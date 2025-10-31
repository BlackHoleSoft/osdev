#include "wasmcool.h"
#include "../std/print.h"

// Decode unsigned 32-bit LEB128 value
uint32_t decode_u32leb128(const uint8_t **data, const uint8_t *end) {
    uint32_t result = 0;
    int shift = 0;
    uint8_t byte;
    
    while (*data < end) {
        byte = **data;
        (*data)++;
        
        // Add the lower 7 bits to the result
        result |= ((uint32_t)(byte & 0x7F)) << shift;
        
        // If the continuation bit is not set, we're done
        if ((byte & 0x80) == 0) {
            break;
        }
        
        shift += 7;
        
        // Check for overflow
        if (shift >= 32) {
            // Error: LEB128 value too large for uint32_t
            return 0;
        }
    }
    
    return result;
}

// Decode signed 32-bit LEB128 value
int32_t decode_i32leb128(const uint8_t **data, const uint8_t *end) {
    uint32_t result = 0;
    int shift = 0;
    uint8_t byte;
    
    while (*data < end) {
        byte = **data;
        (*data)++;
        
        // Add the lower 7 bits to the result
        result |= ((uint32_t)(byte & 0x7F)) << shift;
        shift += 7;
        
        // If the continuation bit is not set, we're done
        if ((byte & 0x80) == 0) {
            break;
        }
    }
    
    // Sign extension if needed
    if (shift < 32 && (byte & 0x40)) {
        result |= (~0U << shift);
    }
    
    return (int32_t)result;
}

// Decode unsigned 64-bit LEB128 value
uint64_t decode_u64leb128(const uint8_t **data, const uint8_t *end) {
    uint64_t result = 0;
    int shift = 0;
    uint8_t byte;
    
    while (*data < end) {
        byte = **data;
        (*data)++;
        
        // Add the lower 7 bits to the result
        result |= ((uint64_t)(byte & 0x7F)) << shift;
        
        // If the continuation bit is not set, we're done
        if ((byte & 0x80) == 0) {
            break;
        }
        
        shift += 7;
        
        // Check for overflow
        if (shift >= 64) {
            // Error: LEB128 value too large for uint64_t
            return 0;
        }
    }
    
    return result;
}

// Decode signed 64-bit LEB128 value
int64_t decode_i64leb128(const uint8_t **data, const uint8_t *end) {
    uint64_t result = 0;
    int shift = 0;
    uint8_t byte;
    
    while (*data < end) {
        byte = **data;
        (*data)++;
        
        // Add the lower 7 bits to the result
        result |= ((uint64_t)(byte & 0x7F)) << shift;
        shift += 7;
        
        // If the continuation bit is not set, we're done
        if ((byte & 0x80) == 0) {
            break;
        }
    }
    
    // Sign extension if needed
    if (shift < 64 && (byte & 0x40)) {
        result |= (~0ULL << shift);
    }
    
    return (int64_t)result;
}
