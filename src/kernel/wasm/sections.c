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

    struct WParsedImportItem items[64];

    if (count > 64) {
        error(ERR_WASM_IMPORT_SIZE, "Imports count > 64");
        return items;
    }
    
    for (int i = 0; i < count; i++) {
        u8* ptr = (u8*)&(content->data.size);
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