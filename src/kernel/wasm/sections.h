#include "../std/types.h"

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

struct WVec {
    u32 size;
    void* data;
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
    u32 value;
    u8 valType;
};

struct WParsedCodePiece {
    u32 size;
    u32 localsCount;
    struct WLocalsItem* locals;
    string code;
};