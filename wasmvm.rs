use std::fs;

// consts
const INSTR_I32_CONST: u8 = 0x41;
const INSTR_I32_ADD: u8 = 0x6a;
const INSTR_END: u8 = 0x0b;
const INSTR_LOCAL_GET: u8 = 0x20;
const INSTR_FUNC: u8 = 0x60;
const INSTR_CALL: u8 = 0x10;

const TYPE_I32: u8 = 0x7f;
const TYPE_FN: u8 = 0x60;

const SEC_CUSTOM: u8 = 0;
const SEC_TYPE: u8 = 1;
const SEC_IMPORT: u8 = 2;
const SEC_FUNC: u8 = 3;
const SEC_TABLE: u8 = 4;
const SEC_MEM: u8 = 5;
const SEC_GLOBAL: u8 = 6;
const SEC_EXPORT: u8 = 7;
const SEC_START: u8 = 8;
const SEC_ELEM: u8 = 9;
const SEC_CODE: u8 = 10;
const SEC_DATA: u8 = 11;

#[derive(Debug)]
struct SecTypeFunction<'a> {
    paramsCnt: u8,
    resultCnt: u8,
    paramsTypes: &'a[u8],
    resultTypes: &'a[u8] 
}

#[derive(Debug)]
struct SecType<'a> {
    typeId: u8,
    size: u8,
    itemsCount: u8,
    functions: &'a[SecTypeFunction<'a>]
}

fn readBinary(path: &str) -> Vec<u8> {
    let contents = fs::read(path).unwrap();
    return contents;
}

fn wasmGetVersion(buffer: &[u8]) -> u8 {
    return buffer[4];
}

fn readSectionType(buffer: &[u8], offset: usize) -> SecType {
    let result = SecType {
        typeId: buffer[offset + 0],
        size: buffer[offset + 1],
        itemsCount: buffer[offset + 2],
        functions: &[],
    };
    return result;
}

fn wasmReadSections(buffer: &[u8]) {
    let mut pointer: usize = 8;
    let mut secSize: u8 = 0;
    //let (secType): (SecType) = (SecType {});

    while pointer < buffer.len() {
        let sectionId: u8 = buffer[pointer];        
        if sectionId == SEC_TYPE {
            let secType = readSectionType(buffer, pointer);
            secSize = secType.size;
            println!("SecType: {:?}", secType);
        } else {
            println!("End of sections!");
        }         

        pointer += (secSize + 2) as usize;
    }
    
}

fn main() {
    let buffer: &[u8] = &[0,97,115,109,1,0,0,0,1,5,1,96,0,1,127,3,2,1,0,10,9,1,7,0,65,5,65,10,106,11,0,10,4,110,97,109,101,2,3,1,0,0];
    let bufLen = buffer.len();

    if bufLen < 8 {
        println!("Binary is empty or has no data!");
        return;
    }

    let version = wasmGetVersion(buffer);
    println!("Binary size: {bufLen}. WASM ver: {version}");

    wasmReadSections(buffer);
}