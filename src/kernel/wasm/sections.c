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

    struct WParsedImportItem* items = mem_10kb();

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
        index = *(u32*)(ptr + bufferOffset);

        items[i].moduleName = moduleName;
        items[i].fnName = fnName;
        items[i].kind = kind;
        items[i].index = index;
    }
    
    return items;
}

struct WParsedExportItem* parseExport(struct WSection* data) {
    struct WVec* vec = parseVec(data->content);
    int count = vec->size;
    u8* ptr = vec->data;

    string fnName = mem_512();
    u8 kind = 0;
    u32 index = 0;
    u16 bufferOffset = 0;

    struct WParsedExportItem* items = mem_10kb();

    if (count > WSECTION_IMPORT_EXPORT_SIZE) {
        error(ERR_WASM_EXPORT_SIZE, "Exports count > 64");
        return items;
    }
    
    for (int i = 0; i < count; i++) {
        // read function name
        struct WVec* nameVec = parseVec(ptr + bufferOffset);
        for (int j = 0; j < nameVec->size; j++) {
            fnName[j] = nameVec->data[j];
        }
        fnName[nameVec->size] = '\0';

        bufferOffset += nameVec->lebSize + nameVec->size;

        // read kind
        kind = ptr[bufferOffset];
        bufferOffset++;

        // read index
        u64 index = 0;
        u8 lebLength = readULeb128(ptr + bufferOffset, &index);

        items[i].fnName = fnName;
        items[i].kind = kind;
        items[i].index = (u32)index;

        bufferOffset += 1 + lebLength;
    }
    
    return items;
}

struct WParsedCodePiece* parseCode(struct WSection* data) {
    struct WVec* vec = parseVec(data->content);
    int count = vec->size;
    u8* ptr = vec->data;

    int maxLocals = 10000 / (32 + 8);
    int localsDeclCount = 0;

    struct WParsedCodePiece* items = mem_1mb();
    struct WLocalsItem* locals = mem_10kb();

    if (count > 64) {
        error(ERR_WASM_CODE_SIZE, "Code items count > 64");
        return items;
    }

    u16 bufferOffset = 0;

    for (int i = 0; i < count; i++) {
        u64 size = 0;
        u8 lebLength = readULeb128(ptr + bufferOffset, &size);
        items[i].size = (u32)size;
        bufferOffset += lebLength;

        u32 startCodeOffset = bufferOffset;

        struct WVec* localsVec = parseVec(ptr + bufferOffset);

        items[i].localsCount = localsVec->size;
        bufferOffset += localsVec->lebSize;

        localsDeclCount += items[i].localsCount;
        if (localsDeclCount > maxLocals) {
            error(ERR_WASM_CODE_SIZE, "Code locals limit reached");
            return items;
        }

        for (int j = 0; j < items[i].localsCount; j++) {
            // parse locals
            u64 locsize = 0;
            u8 locLebLength = readULeb128(ptr + bufferOffset, &locsize);
            locals[j].count = (u32)locsize;
            bufferOffset += locLebLength;

            locals[j].valType = *(ptr + bufferOffset);

            bufferOffset += 1;
        }

        items[i].code = ptr + bufferOffset;

        bufferOffset = startCodeOffset + items[i].size;
    }
    
    return items;
}

u8* findSection(u8* module, u32 moduleSize, u8 sectionType) {
    int bufferOffset = 8;
    
    u8 type = 0;
    u64 size = 0;

    while (bufferOffset < moduleSize) {
        type = *(module + bufferOffset);

        u8 lebLength = readULeb128(module + bufferOffset + 1, &size);

        if (type == sectionType) {
            return module + bufferOffset;
        }

        bufferOffset += 1 + lebLength + size;
    }

    return NULL;
}

struct WSection* parseSection(u8* section) {
    struct WSection* newSection = mem_512();
    u64 size = 0;
    u8 lebLength = 0;

    lebLength = readULeb128(section + 1, &size);

    newSection->id = section[0];
    newSection->size = (u32)size;
    newSection->content = section + 1 + lebLength;

    return newSection;
}

struct WVec* parseVec(u8* data) {
    struct WVec* vec = mem_512();
    u64 size = 0;
    u8 lebLength = 0;

    lebLength = readULeb128(data + 0, &size);

    vec->size = size;
    vec->data = data + lebLength;
    vec->lebSize = lebLength;

    return vec;
}

struct WParsedTypeItem* parseTypes(struct WSection* data) {
    struct WVec* content = parseVec(data->content);
    int count = content->size;

    u32 offset = 0;

    struct WParsedTypeItem* items = mem_10kb();
    u8* rawData = content->data;

    for (int i = 0; i < count; i++) {
        u8 firstByte = rawData[offset];
        if (firstByte != 0x60) {
            error(ERR_WASM_TYPES_OVERFLOW, "Parse types overflow error");
            break;
        }

        items[i].params = parseVec(rawData + offset + 1);
        items[i].results = parseVec(rawData + offset + 1 + items[i].params->lebSize + items[i].params->size);        
    }

    return items;
}

u32* parseFunctions(struct WSection* data) {
    struct WVec* content = parseVec(data->content);
    int count = content->size;

    u32 offset = 0;    

    u32* items = mem_10kb();
    u8* rawData = content->data;

    for (int i = 0; i < count; i++) {
        u64 idx = 0;
        u32 lebLength = readULeb128(rawData + offset, &idx);
        items[i] = idx;

        offset += lebLength;        
    }

    return items;
}

struct WParsedModule* parseModule(u8* module) {
    u32 bufferOffset = 0;

    u8* buffer = mem_10kb();

    u32 maxModuleSize = 1024 * 1000;

    // check magix
    if (module[0] != 0 || module[1] != 0x61 || module[2] != 0x73 || module[3] != 0x6d) {
        error(ERR_WASM_WRONG_MODULE, "Invalid module magic number");
    }

    struct WParsedModule* parsed = (struct WParsedModule*)buffer;
    bufferOffset = 4;

    parsed->version = *(u32*)(module + bufferOffset);

    println(num_to_str((long)parsed->version, 10));

    bufferOffset += 4;

    parsed->sectionTypes = parseSection(findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_TYPE));
    parsed->sectionImports = parseSection(findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_IMPORT));
    parsed->sectionExports = parseSection(findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_EXPORT));
    parsed->sectionFunctions = parseSection(findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_FUNCTION));
    parsed->sectionCode = parseSection(findSection(module, maxModuleSize /*TODO: find real module size*/, WSECTION_ID_CODE));

    parsed->parsedTypes = parseTypes(parsed->sectionTypes);
    parsed->typesCount = parseVec(parsed->sectionTypes->content)->size;

    parsed->parsedFunctions = parseFunctions(parsed->sectionFunctions);
    parsed->functionsCount = parseVec(parsed->sectionFunctions->content)->size;

    parsed->parsedExport = parseExport(parsed->sectionExports);
    parsed->exportCount = parseVec(parsed->sectionExports->content)->size;

    parsed->parsedCode = parseCode(parsed->sectionCode);
    parsed->codePieceCount = parseVec(parsed->sectionCode->content)->size;

    return parsed;
}

void printWTypes(struct WParsedTypeItem* items, int count) {
    print("Types: ");
    for (int i=0; i<count; i++) {
        print("p=[");
        for (int j = 0; j<items[i].params->size; j++) {
            print(num_to_str(items[i].params->data[j], 16));
            print(",");
        }
        print("],r=[");
        for (int j = 0; j<items[i].results->size; j++) {
            print(num_to_str(items[i].results->data[j], 16));
            print(",");
        }
        print("]; ");
    }
}

void printWFunctions(u32* items, int count) {
    print("Fns: [");
    for (int i=0; i<count; i++) {
        print(num_to_str(items[i], 10));
        print(",");
    }
    print("]; ");
}

void printWExports(struct WParsedExportItem* items, int count) {
    print("Exports: [");
    for (int i=0; i<count; i++) {
        print("{");
        print(items[i].fnName);
        print(",kind=");
        print(num_to_str(items[i].kind, 16));
        print(",idx=");
        print(num_to_str(items[i].index, 10));
        print("},");
    }
    print("]; ");
}

void printWCode(struct WParsedCodePiece* items, int count) {
    print("Code: [");
    for (int i=0; i<count; i++) {
        print("{size=");
        print(num_to_str(items[i].size, 10));
        print(",locls=[");

        for (int j = 0; j < items[i].localsCount; j++) {
            print(num_to_str(items[i].locals[j].count, 10));
            print("->");
            print(num_to_str(items[i].locals[j].valType, 16));
            print(",");
        }

        print("],expr=");
        for(int j = 0; j < items[i].size && j < 9; j++) {
            print("#");
            print(num_to_str(items[i].code[j], 16));
        }

        print("},");
    }
    print("]; ");
}

void printWParsedModule(struct WParsedModule* module) {
    print("Module: v=");
    print(num_to_str(module->version, 10));

    print("; types=");
    print(num_to_str(module->sectionTypes->size, 10));

    print("; imports=");
    print(num_to_str(module->sectionImports->size, 10));

    print("; exports=");
    print(num_to_str(module->sectionExports->size, 10));

    print("; fun=");
    print(num_to_str(module->sectionFunctions->size, 10));

    print("; code=");
    print(num_to_str(module->sectionCode->size, 10));

    println("");
}

