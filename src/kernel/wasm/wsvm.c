#include "./wsvm.h"
#include "../std/string.h"
#include "../std/mem.h"
#include "../std/print.h"

struct WParsedExportItem* findMainFn(struct WParsedModule* md) {
    int itemCounter = 0;

    struct WParsedExportItem* items = md->parsedExport;

    for (int i = 0; i < md->exportCount; i++) {
        if (str_compare(items[i].fnName, "main")) {
            return &items[i];
        }
    }

    return NULL;
}

u64 wsvmCall(struct WParsedCodePiece* codePiece, struct WParsedTypeItem* type, u64* stack, u32 stackPtr) {
    u8* code = codePiece->code;
    u32 localStackPtr = stackPtr;

    u8 cmd = 0;
    u64 cmdArg = 0;    

    while (cmd != WINSTR_END) {
        cmd = code[0];

        if (localStackPtr < 0) {
            error(ERR_WASM_EXEC_STACKOVERFLOW, "WASM stack overflow");
            break;
        }

        switch (cmd)
        {
            case WINSTR_CONST_I32:
            {
                localStackPtr--;
                u8 lebSize = readULeb128(code + 1, &cmdArg);
                stack[localStackPtr] = cmdArg;
                code += 1 + lebSize;
            }
            break;

            case WINSTR_LOCAL_GET:
            {
                // parameters be first
                u8 lebSize = readULeb128(code + 1, &cmdArg);
                localStackPtr--;
                if (cmdArg < type->params->size) {
                    // params
                    stack[localStackPtr] = stack[stackPtr + type->params->size - cmdArg - 1];
                } else {
                    // locals
                    stack[localStackPtr] = stack[stackPtr - (cmdArg - type->params->size) - 1];
                }
                code += 1 + lebSize;
            }
            break;
            
            case WINSTR_I32_ADD: 
            {
                u64 argA = stack[localStackPtr];
                u64 argB = stack[localStackPtr + 1];
                localStackPtr++;
                stack[localStackPtr] = argA + argB;
                code++;
            }
            break;

            default:
                break;
        }
    }

    return stack[localStackPtr];
}

u64 wsvmExecute(struct WParsedModule* module) {

    struct WParsedExportItem* mainFn = findMainFn(module);

    if (mainFn == NULL) {
        error(ERR_WASM_EXEC_NOMAIN, "The main function not found");
        return ERR_WASM_EXEC_NOMAIN;
    }

    struct WParsedCodePiece* codePiece = &module->parsedCode[mainFn->index];
    struct WParsedTypeItem* type = &module->parsedTypes[mainFn->index];

    u64* stack = mem_10kb();
    // ptr at the end of memory block
    // stack grows backward
    u32 stackPtr = (10 * 1024 - 128) / 8;

    u64 result = wsvmCall(codePiece, type, stack, stackPtr);

    return result;
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

    println("Start execution...");
    u64 result = wsvmExecute(parsedModule);
    print("End execution: ");
    println(num_to_str(result, 10));

    return 0;
}