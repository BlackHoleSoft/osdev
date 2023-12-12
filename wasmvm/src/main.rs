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

const IMPORT_FN_LOG: u8 = 0;
const IMPORT_FN_GET_MEM: u8 = 1;
const IMPORT_FN_SET_MEM: u8 = 2;

static mut MEMORY: [u8; 256] = [0; 256];


#[derive(Debug)]
struct VEC<T> {
    arr: [Option<T>; 32],
    pointer: i32
}

#[derive(Debug)]
struct Sections {
    sec_type: SecType,
    sec_func: SecFunctions,
    sec_code: SecCode,
    sec_import: SecImport
}

#[derive(Debug)]
struct SecImportItem {
    signature_id: u8,
    kind: u8,
    empty: bool,
}

#[derive(Debug)]
struct SecImport {
    type_id: u8,
    size: u8,
    items_count: u8,
    items: Vec<SecImportItem>
}

#[derive(Debug)]
struct SecFunctions {
    type_id: u8,
    size: u8,
    items_count: u8,
    data: Vec<u8>,
}

#[derive(Debug)]
struct SecTypeFunction {
    params_cnt: u8,
    result_cnt: u8,
    params_types: Vec<u8>,
    result_types: Vec<u8>,    
}

#[derive(Debug)]
struct SecType {
    type_id: u8,
    size: u8,
    items_count: u8,
    functions: Vec<SecTypeFunction>
}

#[derive(Debug)]
struct SecCodeFn {
    body_size: u8,
    locals_count: u8,
    code_ptr: u8,
    locals: Vec<u8>
}

#[derive(Debug)]
struct SecCode {
    type_id: u8,
    size: u8,
    items_count: u8,
    functions: Vec<SecCodeFn>
}

/******************/
/***** Utils ******/
/******************/

fn vec_new<T>() -> VEC<T> {
    return VEC {
        arr: [None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None],
        pointer: -1
    };
}

fn vec_len<T>(vec: &VEC<T>) -> i32 {
    return vec.pointer + 1;
}

fn vec_push<T>(mut vec: VEC<T>, item: T) -> VEC<T> {    
    vec.pointer += 1;
    vec.arr[vec.pointer as usize] = Some(item);
    return vec;
}

fn vec_get<T>(vec: &VEC<T>, index: usize) -> &Option<T> {    
        if index > vec.pointer as usize || vec.arr[vec.pointer as usize].is_none() {
            return &None;
        }
        let result = &vec.arr[index];

        return result;
}

fn leb_decode_unsigned(buffer: &[u8], pos: usize) -> u64 {
    let mut result: u64 = 0;
    let mut shift = 0;
    let mut pointer = pos;
    loop {
        let byte = buffer[pointer];
        result |= ((byte & !(1 << 7)) as u64) << shift;
        if byte & (1 << 7) == 0 {
            break;
        }
        shift += 7;
        pointer += 1;
    }
    return result;
}

fn leb_encode_unsigned(v: u64) -> (usize, [u8; 8]) {
    let mut result = [0; 8];
    let mut size: usize = 0;
    let mut value = v;

    if value == 0 {
        return (1, result);
    }

    while value != 0 {
        let mut byte: u8 = (value & !(1 << 7)) as u8;
        value >>= 7;
        if value != 0 {
            byte |= 1 << 7;
        }
        result[size] = byte;
        size += 1;        
    }
    return (size, result);
}

fn read_binary(path: &str) -> Vec<u8> {
    let contents = fs::read(path).unwrap();
    return contents;
}

fn wasm_get_version(buffer: &[u8]) -> u8 {
    return buffer[4];
}

/******************/
/**** Sections ****/
/******************/

fn read_section_type(buffer: &[u8], offset: usize) -> SecType {
    let items_cnt = buffer[offset + 2];
    let mut fn_vec = Vec::new();
    let mut index = 0;

    while fn_vec.len() < items_cnt as usize {        
        let params_cnt: u8 = buffer[offset + index + 4];
        let mut params_vec = Vec::new();
        let mut p_i = 0;
        while p_i < params_cnt {
            params_vec.push(buffer[offset + index + 5 + p_i as usize]);
            p_i += 1;
        }

        let result_cnt: u8 = buffer[offset + index + 5 + params_cnt as usize];
        let mut ret_vec = Vec::new();
        let mut r_i = 0;
        while r_i < result_cnt {
            ret_vec.push(buffer[offset + index + 6 + params_cnt as usize + r_i as usize]);
            r_i += 1
        }

        fn_vec.push(SecTypeFunction {
            params_cnt: params_cnt,
            result_cnt: result_cnt,
            params_types: params_vec,
            result_types: ret_vec,
        });
        index += (params_cnt + result_cnt + 3) as usize;
    }

    let result = SecType {
        type_id: buffer[offset + 0],
        size: buffer[offset + 1],
        items_count: items_cnt,
        functions: fn_vec
    };
    
    return result;
}

fn read_section_import(buffer: &[u8], offset: usize) -> SecImport {
    let sec_size = buffer[offset + 1];
    let items_cnt = buffer[offset + 2];
    let mut items = Vec::new();
    let mut i = 0;
    let mut index = 0;

    while i < items_cnt {
        let module_name_len = buffer[offset + 3 + index + 0];
        let fn_name_len = buffer[offset + 3 + index + 1 + module_name_len as usize];
        let fn_kind = buffer[offset + 3 + index + 2 + (module_name_len + fn_name_len) as usize];
        let fn_id = buffer[offset + 3 + index + 3 + (module_name_len + fn_name_len) as usize];

        items.push(SecImportItem {
            kind: fn_kind,
            signature_id: fn_id,
            empty: false
        });

        i += 1;
        index += 4 + (module_name_len + fn_name_len) as usize;
    }

    return SecImport {
        type_id: SEC_IMPORT,
        size: sec_size,
        items_count: items_cnt,
        items: items
    };
}

fn read_section_fun(buffer: &[u8], offset: usize) -> SecFunctions {
    let sec_size = buffer[offset + 1];
    let items_cnt = buffer[offset + 2];
    let mut index = 0;
    let mut fn_vec = Vec::new();

    while index < items_cnt {
        fn_vec.push(buffer[offset + 3 + index as usize]);
        index += 1;
    }

    return SecFunctions {
        type_id: SEC_FUNC,
        size: sec_size,
        items_count: items_cnt,
        data: fn_vec
    };
}

fn read_section_code(buffer: &[u8], offset: usize) -> SecCode {
    let sec_size = buffer[offset + 1];
    let items_cnt = buffer[offset + 2];
    let mut fns = Vec::new();
    let mut index = 0;
    let mut i = 0;

    while i < items_cnt {
        let fn_size = buffer[offset + 3 + index + 0];
        let fn_locals = buffer[offset + 3 + index + 1];
        let mut locals_vec = Vec::new();

        while locals_vec.len() < fn_locals as usize {
            locals_vec.push(buffer[offset + 3 + index + 2 + locals_vec.len()]);
        }

        fns.push(SecCodeFn {
            body_size: fn_size,
            locals_count: fn_locals,
            code_ptr: (offset + 3 + index) as u8 + 2 + fn_locals,
            locals: locals_vec
        });
        index += fn_size as usize + 1;
        i += 1;
    }

    return SecCode {
        size: sec_size,
        type_id: SEC_CODE,
        items_count: items_cnt,
        functions: fns
    };
}

fn wasm_read_sections(buffer: &[u8]) -> Sections {
    let mut pointer: usize = 8;
    let mut sec_size: u8 = 0;
    let mut sec_type = SecType { type_id: 0, size: 0, items_count: 0, functions: Vec::new() };
    let mut sec_fn = SecFunctions { type_id: 0, size: 0, items_count: 0, data: Vec::new() };
    let mut sec_code = SecCode { type_id: 0, items_count: 0, size: 0, functions: Vec::new() };
    let mut sec_import = SecImport { type_id: 0, size: 0, items_count: 0, items: Vec::new() };
    //let (secType): (SecType) = (SecType {});

    while pointer < buffer.len() {
        let section_id: u8 = buffer[pointer];        
        if section_id == SEC_TYPE {
            sec_type = read_section_type(buffer, pointer);
            sec_size = sec_type.size;
        } else if section_id == SEC_FUNC {
            sec_fn = read_section_fun(buffer, pointer);
            sec_size = sec_fn.size;
        } else if section_id == SEC_CODE {
            sec_code = read_section_code(buffer, pointer);
            sec_size = sec_code.size;
        } else if section_id == SEC_IMPORT {
            sec_import = read_section_import(buffer, pointer);
            sec_size = sec_import.size;
        } else {
            println!("End of sections!");
            break;
        }         

        pointer += (sec_size + 2) as usize;
    }

    let sections = Sections { 
        sec_type: sec_type,
        sec_func: sec_fn,
        sec_code: sec_code,
        sec_import: sec_import
    };
    println!("Sections: {:?}", sections);
    return sections;
    
}

/******************/
/***** VM Loop ****/
/******************/

struct VMStack {
    stack: [i64; 1024],
    length: usize,
}

fn stack_push(mut stack: VMStack, value: i64) -> VMStack {    
    stack.stack[stack.length] = value;
    stack.length += 1;
    return stack;
}

fn stack_pop(mut stack: VMStack) -> (i64, VMStack) {
    stack.length -= 1;
    return (stack.stack[stack.length], stack);
}

fn vm_loop(buffer: &[u8], sections: &Sections, start_ptr: u8, params: Vec<i64>) -> i64 {
    let mut pointer = start_ptr as usize;
    let mut stack: VMStack = VMStack { 
        stack: [0; 1024],
        length: 0 
    };

    while pointer < buffer.len() {
        let cmd = buffer[pointer + 0];
        let param = buffer[pointer + 1];

        let stack_slice: &[i64] = &stack.stack[0..stack.length];
        println!("Cmd: {cmd}; Param: {param}; Stack: {:?}", stack_slice);

        if cmd == INSTR_END {
            println!("End of function");
            break;
        } else if cmd == INSTR_I32_CONST {
            let value = leb_decode_unsigned(buffer, pointer + 1);  
            stack = stack_push(stack, value as i64);
            pointer += 2;
        } else if cmd == INSTR_LOCAL_GET {
            stack = stack_push(stack, params[param as usize]);
            pointer += 2;
        } else if cmd == INSTR_I32_ADD {
            let (p1, st) = stack_pop(stack);
            let (p2, st) = stack_pop(st);
            stack = st;
            println!("Add: {p1} + {p2}");
            stack = stack_push(stack, p1 + p2);
            pointer += 1;
        } else if cmd == INSTR_CALL {
            let (fn_data, code_index, import_index) = get_fn_from_sections(sections, param);
            let mut i = fn_data.params_cnt;
            let mut params = Vec::new();
            while i > 0 {
                let (v, st) = stack_pop(stack);
                stack = st;
                params.push(v);
                i -= 1;
            }
            let result = instr_call(buffer, sections, param, code_index, import_index, params);
            if fn_data.result_cnt > 0 {
                stack = stack_push(stack, result);
            }
            pointer += 2;
        } else {
            pointer += 1;
        }        
    }

    if stack.length > 0 {
        return stack_pop(stack).0;
    } else {
        return 0;
    }
}

fn get_fn_from_sections(sections: &Sections, fn_id: u8) -> (&SecTypeFunction, i16, i16) {
    let mut types: Vec<&SecTypeFunction> = Vec::new();

    for i in &sections.sec_import.items {
        types.push(&sections.sec_type.functions[i.signature_id as usize]);
    }
    for i in &sections.sec_func.data {
        types.push(&sections.sec_type.functions[*i as usize]);
    }

    let mut fn_code = -1;
    let mut fn_import = -1;
    let fn_type = types[fn_id as usize];

    if fn_id >= sections.sec_import.items_count {
        fn_code = (fn_id - sections.sec_import.items_count) as i16;
    } else {
        fn_import = fn_id as i16;
    }    

    return (fn_type, fn_code, fn_import);
}

fn instr_call(buffer: &[u8], sections: &Sections, fn_id: u8, code_index: i16, import_index: i16, params: Vec<i64>) -> i64 {
    println!("Call function: {fn_id}; {:?}", params);        
    
    let mut all_params = Vec::new();
    let mut i = params.len();
    while i > 0 {
        i -= 1;
        all_params.push(params[i]);
    }

    if code_index >= 0 {
        let fn_code = &sections.sec_code.functions[code_index as usize];
        for i in &fn_code.locals {
            all_params.push(*i as i64);
        }
    
        return vm_loop(buffer, sections, fn_code.code_ptr, all_params);
    } else {
        // Call imported function
        println!("Call imported function: {import_index}");
        return call_imported(fn_id, all_params);
    }    
}

fn call_imported(fn_id: u8, params: Vec<i64>) -> i64{
    if fn_id == IMPORT_FN_LOG {
        api_log(params[0]);
        return 0;
    } else if fn_id == IMPORT_FN_GET_MEM {
        return api_get_mem(params[0]) as i64;
    } else if fn_id == IMPORT_FN_SET_MEM {
        api_set_mem(params[0], params[1] as u8);
        return 0;
    } else {
        println!("Couldn't find imported function: {fn_id}");
        return 0;
    }
}

fn start_vm(buffer: &[u8], sections: &Sections) {
    println!("Start VM...");
    let loop_result = vm_loop(buffer, sections, sections.sec_code.functions.as_slice()[0].code_ptr, Vec::new());
    println!("Start function returned: {loop_result}");
}

/******************/
/**** API Calls ***/
/******************/

fn api_log(value: i64) {
    println!("From VM: {value}");
}

fn api_get_mem(addr: i64) -> u8 {
    /*unsafe {
        return MEMORY[addr as usize];
    }*/
    println!("GET MEMORY: {addr}");
    return 0;
}

fn api_set_mem(addr: i64, value: u8) {
    /*unsafe {
        MEMORY[addr as usize] = value;
    }*/   
    println!("SET MEMORY: {addr}; {value}"); 
}

fn main() {
    let buffer: &[u8] = &[0,97,115,109,1,0,0,0,1,19,4,96,1,127,0,96,1,127,1,127,96,2,127,127,0,96,0,1,127,2,43,3,5,105,110,100,101,120,3,108,111,103,0,0,5,105,110,100,101,120,6,103,101,116,77,101,109,0,1,5,105,110,100,101,120,6,115,101,116,77,101,109,0,2,3,2,1,3,10,22,1,20,0,65,128,128,46,65,3,16,2,65,129,128,46,65,12,16,2,65,0,11,0,85,4,110,97,109,101,1,67,3,0,18,97,115,115,101,109,98,108,121,47,105,110,100,101,120,47,108,111,103,1,21,97,115,115,101,109,98,108,121,47,105,110,100,101,120,47,103,101,116,77,101,109,2,21,97,115,115,101,109,98,108,121,47,105,110,100,101,120,47,115,101,116,77,101,109,2,9,4,0,0,1,0,2,0,3,0];
    let buf_len = buffer.len();

    /*println!("LEB128 read unsigned: {:?}", leb_encode_unsigned(leb_decode_unsigned(&[17, 3, 0, 6, 0, 0], 0)));
    println!("LEB128 read unsigned: {:?}", leb_encode_unsigned(leb_decode_unsigned(&[0x80, 0x08, 0, 6, 0, 0], 0)));
    println!("LEB128 read unsigned: {:?}", leb_encode_unsigned(leb_decode_unsigned(&[0x81, 0xc7, 0x07, 6, 0, 0], 0)));*/

    let mut test_vec: VEC<i32> = vec_new();
    println!("Vec created");
    test_vec = vec_push(test_vec, 777);
    test_vec = vec_push(test_vec, 666);
    //test_vec = vec_push(test_vec, 888);
    println!("Vec items pushed");
    let val = *vec_get(&test_vec, 1);
    let disp_val: i32 = val.expect("Err");
    println!("Vec item get: {disp_val}");
    let len = vec_len(&test_vec);

    println!("Len: {len}");

    if buf_len < 8 {
        println!("Binary is empty or has no data!");
        return;
    }

    let version = wasm_get_version(buffer);
    println!("Binary size: {buf_len}. WASM ver: {version}");

    let sections = wasm_read_sections(buffer);
    start_vm(buffer, &sections);

    unsafe {
        println!("Memory: {:?}", MEMORY);
    }
}