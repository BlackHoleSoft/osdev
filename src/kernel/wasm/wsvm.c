#include "./wsvm.h"
#include "../std/string.h"
#include "../std/mem.h"
#include "../std/print.h"

u32 findMainFnIndex(struct WParsedModule* md) {
    int itemCounter = 0;

    struct WParsedExportItem* items = parseExport(md->sectionExports);

    while (itemCounter < WSECTION_IMPORT_EXPORT_SIZE) {
        if (str_compare(items[itemCounter].fnName, "main")) {
            return items[itemCounter].index;
        }
    }

    return 0;
}

struct WParsedCodePiece getFnCode(struct WParsedModule* md, u32 index) {
    struct WParsedCodePiece* items = parseCode(md->sectionCode);
    return items[index];    
}

int wsvmStart(u8* module) {
    struct WParsedModule* parsedModule = parseModule(module);

    if (parsedModule->version != 1) {
        return ERR_WASM_WRONG_MODULE;
    }

    printWParsedModule(parsedModule);
    printWTypes(parsedModule->parsedTypes, parsedModule->typesCount);
    printWFunctions(parsedModule->parsedFunctions, parsedModule->functionsCount);
    printWExports(parsedModule->parsedExport, parsedModule->exportCount);
    printWCode(parsedModule->parsedCode, parsedModule->codePieceCount);

    return 0;
}