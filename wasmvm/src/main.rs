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
struct Sections {
    sec_type: SecType,
    sec_func: SecFunctions,
    sec_code: SecCode,
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
            ret_vec.push(buffer[offset + index + 6 + r_i as usize]);
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
        } else {
            println!("End of sections!");
            break;
        }         

        pointer += (sec_size + 2) as usize;
    }

    let sections = Sections { sec_type: sec_type, sec_func: sec_fn, sec_code: sec_code };
    println!("Sections: {:?}", sections);
    return sections;
    
}

/******************/
/***** VM Loop ****/
/******************/

struct VMStack {
    stack: [u8; 1024],
    length: usize,
}

fn stack_push(mut stack: VMStack, value: u8) -> VMStack {    
    stack.stack[stack.length] = value;
    stack.length += 1;
    return stack;
}

fn stack_pop(mut stack: VMStack) -> (u8, VMStack) {
    stack.length -= 1;
    return (stack.stack[stack.length], stack);
}

fn vm_loop(buffer: &[u8], start_ptr: u8, params: &[u8]) -> u8 {
    let mut pointer = start_ptr as usize;
    let mut stack: VMStack = VMStack { 
        stack: [0; 1024],
        length: 0 
    };

    while pointer < buffer.len() {
        let cmd = buffer[pointer + 0];
        let param = buffer[pointer + 1];

        let stack_slice: &[u8] = &stack.stack[0..stack.length];
        println!("Cmd: {cmd}; Param: {param}; Stack: {:?}", stack_slice);

        if cmd == INSTR_END {
            println!("End of function");
            break;
        } else if cmd == INSTR_I32_CONST {  
            stack = stack_push(stack, param);
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

fn instr_call(buffer: &[u8], sections: &Sections, fn_id: u8, params: &[u8]) -> u8 {
    let fn_index = sections.sec_func.data.iter().position(|&f| f == fn_id).unwrap();
    let code = &sections.sec_code.functions.as_slice()[fn_index];
    return vm_loop(buffer, code.code_ptr, params);
}

fn start_vm(buffer: &[u8], sections: &Sections) {
    println!("Start VM...");
    let loop_result = vm_loop(buffer, sections.sec_code.functions.as_slice()[0].code_ptr, &[]);
    println!("Start function returned: {loop_result}");
}

fn main() {
    let buffer: &[u8] = &[0,97,115,109,1,0,0,0,1,5,1,96,0,1,127,3,2,1,0,10,9,1,7,0,65,5,65,10,106,11,0,10,4,110,97,109,101,2,3,1,0,0];
    let buf_len = buffer.len();

    if buf_len < 8 {
        println!("Binary is empty or has no data!");
        return;
    }

    let version = wasm_get_version(buffer);
    println!("Binary size: {buf_len}. WASM ver: {version}");

    let sections = wasm_read_sections(buffer);
    start_vm(buffer, &sections);
}