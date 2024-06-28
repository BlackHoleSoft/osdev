#include "../std/types.h"
#include "leb.h"

#define WSECTION_ID_CUSTOM 0
#define WSECTION_ID_TYPE 1
#define WSECTION_ID_IMPORT 2
#define WSECTION_ID_FUNCTION 3
#define WSECTION_ID_TABLE 4
#define WSECTION_ID_MEMORY 5
#define WSECTION_ID_GLOBAL 6
#define WSECTION_ID_EXPORT 7
#define WSECTION_ID_START 8
#define WSECTION_ID_ELEMENT 9
#define WSECTION_ID_CODE 10
#define WSECTION_ID_DATA 11

#define WSECTION_IMPORT_EXPORT_SIZE 64

struct WVec {
    u32 size;
    u8 lebSize;
    u8* data;
};

struct WSection {
    u8 id;
    u32 size;
    void* content;
};

struct WSectionTypeContent {
    struct WVec functions;
};

struct WSectionVecContent {
    struct WVec data;
};

struct WSectionStartContent {
    u32 fnIndex;
};

struct WParsedTypeItem {
    struct WVec* params;
    struct WVec* results;
};

struct WParsedImportItem {
    string moduleName;
    string fnName;
    u8 kind;
    u32 index;
};

struct WParsedExportItem {
    string fnName;
    u8 kind;
    u32 index;
};

struct WLocalsItem {
    u32 count;
    u8 valType;
};

struct WParsedCodePiece {
    u32 size;
    u32 localsCount;
    struct WLocalsItem* locals;
    string code;
};

struct WParsedModule {
    u32 version;
    struct WSection* sectionTypes;
    struct WSection* sectionFunctions;
    struct WSection* sectionImports;
    struct WSection* sectionExports;
    struct WSection* sectionCode;
    
    struct WParsedTypeItem* parsedTypes;
    u32* parsedFunctions;
    struct WParsedImportItem* parsedImport;
    struct WParsedExportItem* parsedExport;
    struct WParsedCodePiece* parsedCode;

    u32 typesCount;
    u32 importCount;
    u32 exportCount;
    u32 codePieceCount;
    u32 functionsCount;
};

struct WVec* parseVec(u8* data);

struct WParsedImportItem* parseImport(struct WSection* data);

struct WParsedExportItem* parseExport(struct WSection* data);

struct WParsedCodePiece* parseCode(struct WSection* data);

u8* findSection(u8* module, u32 moduleSize, u8 sectionType);

struct WParsedModule* parseModule(u8* module);

void printWTypes(struct WParsedTypeItem* items, int count);

void printWFunctions(u32* items, int count);

void printWExports(struct WParsedExportItem* items, int count);

void printWCode(struct WParsedCodePiece* items, int count);

void printWParsedModule(struct WParsedModule* module);