#include "sections.h"
#include "../std/mem.h"
#include "../std/err.h"

struct WParsedImportItem* parseImport(struct WSection* data) {
    struct WSectionVecContent* content = (struct WSectionVecContent*)data->content;
    int count = content->data.size;

    string moduleName = mem_512();
    string fnName = mem_512();
    u8 kind = 0;
    u32 index = 0;
    u16 bufferOffset = 0;

    struct WParsedImportItem items[WSECTION_IMPORT_EXPORT_SIZE];

    if (count > WSECTION_IMPORT_EXPORT_SIZE) {
        error(ERR_WASM_IMPORT_SIZE, "Imports count > 64");
        return items;
    }
    
    for (int i = 0; i < count; i++) {
        u8* ptr = (u8*)&(content->data.data);
        // read module name
        u8 strLength = ptr[bufferOffset];
        bufferOffset++;
        for (int j = 0; j < strLength; j++) {
            moduleName[j] = ptr[bufferOffset + j];
        }
        moduleName[strLength] = '\0';
        bufferOffset += strLength;

        // read function name
        strLength = ptr[bufferOffset];
        bufferOffset++;
        for (int j = 0; j < strLength; j++) {
            fnName[j] = ptr[bufferOffset + j];
        }
        fnName[strLength] = '\0';
        bufferOffset += strLength;

        // read kind
        kind = ptr[bufferOffset];
        bufferOffset++;

        // read index
        index = *(u32*)(ptr[bufferOffset]);

        items[i].moduleName = moduleName;
        items[i].fnName = fnName;
        items[i].kind = kind;
        items[i].index = index;
    }
    
    return items;
}

struct WParsedExportItem* parseExport(struct WSection* data) {
    struct WSectionVecContent* content = (struct WSectionVecContent*)data->content;
    int count = content->data.size;

    string fnName = mem_512();
    u8 kind = 0;
    u32 index = 0;
    u16 bufferOffset = 0;

    struct WParsedExportItem items[WSECTION_IMPORT_EXPORT_SIZE];

    if (count > WSECTION_IMPORT_EXPORT_SIZE) {
        error(ERR_WASM_EXPORT_SIZE, "Exports count > 64");
        return items;
    }
    
    for (int i = 0; i < count; i++) {
        u8* ptr = (u8*)&(content->data.data);       

        // read function name
        u8 strLength = ptr[bufferOffset];
        bufferOffset++;
        for (int j = 0; j < strLength; j++) {
            fnName[j] = ptr[bufferOffset + j];
        }
        fnName[strLength] = '\0';
        bufferOffset += strLength;

        // read kind
        kind = ptr[bufferOffset];
        bufferOffset++;

        // read index
        index = *(u32*)(ptr[bufferOffset]);

        items[i].fnName = fnName;
        items[i].kind = kind;
        items[i].index = index;
    }
    
    return items;
}

struct WParsedCodePiece* parseCode(struct WSection* data) {
    struct WSectionVecContent* content = (struct WSectionVecContent*)data->content;
    int count = content->data.size;

    int maxLocals = 10000 / (32 + 8);
    int localsDeclCount = 0;

    struct WParsedCodePiece items[64];
    struct WLocalsItem* locals = mem_10kb();

    if (count > 64) {
        error(ERR_WASM_CODE_SIZE, "Code items count > 64");
        return items;
    }

    u16 bufferOffset = 0;

    for (int i = 0; i < count; i++) {       
        u8* ptr = (u8*)&(content->data.data);
        
        items[i].size = *(u32*)(ptr + bufferOffset);
        bufferOffset += 4;
        items[i].localsCount = *(u32*)(ptr + bufferOffset);
        bufferOffset += 4;

        localsDeclCount += items[i].localsCount;
        if (localsDeclCount > maxLocals) {
            error(ERR_WASM_CODE_SIZE, "Code locals limit reached");
            return items;
        }

        for (int j = 0; j < items[i].localsCount; j++) {
            // parse locals
            locals[j].count = *(u32*)(ptr + bufferOffset);
            bufferOffset += 4;

            locals[j].valType = *(ptr + bufferOffset);            

            bufferOffset += 1;
        }

        items[i].code = ptr + bufferOffset;

        bufferOffset += items[i].size - items[i].localsCount * (4 + 1);
    }
    
    return items;
}

struct WSection* findSection(u8* module, u32 moduleSize, u8 sectionType) {
    int bufferOffset = 8;
    
    u8 type = 0;
    u32 size = 0;

    while (bufferOffset < moduleSize) {
        type = *(module + bufferOffset);
        bufferOffset += 1;
        size = *(u32*)(module + bufferOffset);

        if (type == sectionType) {
            return (struct WSection*)(module + bufferOffset);
        }

        bufferOffset += size;
    }

    return NULL;
}

struct WParsedModule* parseModule(u8* module) {
    u32 bufferOffset = 0;

    u8* buffer = mem_1mb();

    u32 maxModuleSize = 1024 * 1000;

    // check magix
    if (buffer[0] != 0 || buffer[1] != 0x61 || buffer[2] != 0x73 || buffer[3] != 0x6d) {
        error(ERR_WASM_WRONG_MODULE, "Invalid module magic number");
    }

    struct WParsedModule* parsed = (struct WParsedModule*)buffer;
    u32 bufferoffset = 4;

    parsed->version = *(u32*)(buffer + bufferOffset);

    bufferOffset += 4;

    parsed->sectionTypes = findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_TYPE);
    parsed->sectionImports = findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_IMPORT);
    parsed->sectionExports = findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_EXPORT);
    parsed->sectionFunctions = findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_FUNCTION);
    parsed->sectionCode = findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_CODE);

    return parsed;
}