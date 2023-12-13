const fs = require('fs');

console.log('Run test wasm vm');

// consts
const INSTR_I32_CONST = 0x41;
const INSTR_I32_ADD = 0x6a;
const INSTR_END = 0x0b;
const INSTR_LOCAL_GET = 0x20;
const INSTR_FUNC = 0x60;
const INSTR_CALL = 0x10;

const TYPE_I32 = 0x7f;
const TYPE_FN = 0x60;

const SEC_CUSTOM = 0;
const SEC_TYPE = 1;
const SEC_IMPORT = 2;
const SEC_FUNC = 3;
const SEC_TABLE = 4;
const SEC_MEM = 5;
const SEC_GLOBAL = 6;
const SEC_EXPORT = 7;
const SEC_START = 8;
const SEC_ELEM = 9;
const SEC_CODE = 10;
const SEC_DATA = 11;

const typeFunctions = {
    [TYPE_FN]: getFuncType,
}

function getBinary(base64) {
    let arr = [...Buffer.from(base64, 'base64')];
    let buf = {
        arr,
        length: arr.length
    };
    buf.gb = function (i) {
        return '0x' + this.arr[i].toString(16);
    };
    buf.gb.bind(buf);

    buf.int = function (pos, length) {
        let str = "";
        for (let i=0; i<length; i++) {
            str += this.arr[pos + i].toString(16);
        }
        return Number.parseInt("0x" + str);
    }
    buf.int.bind(buf);

    return buf;
}

function to0x(i) {
    return i.toString(16);
}

function encodeSInt32(value) {
    value |= 0;
    const result = [];
    while (true) {
      const byte_ = value & 0x7f;
      value >>= 7;
      if (
        (value === 0 && (byte_ & 0x40) === 0) ||
        (value === -1 && (byte_ & 0x40) !== 0)
      ) {
        result.push(byte_);
        return result;
      }
      result.push(byte_ | 0x80);
    }
};

function encodeUInt32(value) {    
    return [value];
};

function decodeSInt32(buffer, index) {
    let result = 0;
    let shift = 0;
    let input = buffer.arr.slice(index, index + 4);
    let len = 0;
    while (true) {
      len++;
      const byte = input.shift();
      result |= (byte & 0x7f) << shift;
      shift += 7;
      if ((0x80 & byte) === 0) {
        if (shift < 32 && (byte & 0x40) !== 0) {
          return [result | (~0 << shift), len];
        }
        return [result, len];
      }
    }
};

function decodeUInt32(input) {    
    return input[0];
};

function getWasmMeta(buf) {
    const magic = [buf.gb(0), buf.gb(1), buf.gb(2), buf.gb(3)];
    console.log('magic:', magic);
    const version = buf.int(4, 1);
    console.log('version:', version);

    return {
        magic, version
    }
}

function getFuncType(buffer, offset) {
    const paramsCnt = decodeUInt32([buffer.arr[offset + 1]]);
    const params = [];
    for (let i=0; i<paramsCnt; i++) {
        params.push(buffer.arr[offset + 2 + i]);
    }
    const retCnt = decodeUInt32([buffer.arr[offset + 2 + paramsCnt]]);
    const retType = retCnt > 0 ? buffer.arr[offset + 2 + paramsCnt + 1] : null;
    return {
        params,
        retType,
        length: 2 + paramsCnt + 1 + retCnt
    };
}

function runvm(buffer, imports = {}) {
    console.log('Run vm...');

    const sections = [];

    function add(stack) {
        let res = stack.pop() + stack.pop();
        stack.push(res);
    }

    function call(id, stack) {
        console.log('CALL', id, stack);
        const fnSec = sections.find(f => f.typeId === SEC_FUNC);
        const typeSec = sections.find(f => f.typeId === SEC_TYPE);        
        const codeSec = sections.find(f => f.typeId === SEC_CODE);
        
        const fnIndex = fnSec.functions.indexOf(id); 
        const {params, retType} = typeSec.types[id];

        if (fnIndex === -1) {
            // call from imports
            const importFn = sections.find(f => f.typeId === SEC_IMPORT)?.imports.find(im => im.signatureIndex === id);
            if (importFn) {
                const module = imports[importFn.moduleName];
                const fn = module ? module[importFn.fnName] : null;
                if (fn) {
                    console.log('Call extern function:', importFn, stack);
                    const fnParams = [];
                    for (let i=0; i<params.length; i++) {
                        fnParams.unshift(stack.pop());
                    }
                    const result = fn(...fnParams);
                    if (retType) stack.push(result);
                } else {
                    throw new Error("Couldn't execute external function. Module: " + importFn.moduleName + "." + importFn.fnName);
                }
            } else {
                throw new Error("Couldn't find function inside imports. Fn id=" + id);
            }
        } else {
            const fnBlock = codeSec?.blocks[fnIndex];
            if (fnBlock) {
                const fnParams = [];
                for (let i=0; i<params.length; i++) {
                    fnParams.unshift(stack.pop());
                }
    
                const result = fun(fnBlock.codePointer, fnParams);
                if (retType) {
                    stack.push(result);
                }
            } else {
                throw new Error("Couldn't find function code block. Fn id = " + id);
            }
        }        
    }

    function fun(addr, params) {
        const stack = [];
        let pointer = addr;

        while (pointer < buffer.length && buffer.arr[pointer] != INSTR_END) {
            const command = buffer.arr[pointer];            

            switch (command) {
                case INSTR_I32_CONST:
                    console.log('Cmd:', '0x' + command.toString(16), "Param:", buffer.arr[pointer + 1], "Stack:", stack);
                    const [val, len] = decodeSInt32(buffer, pointer + 1);
                    console.log('Decoded:', val, len);
                    stack.push(val);
                    pointer += 1 + len;
                    break;
                case INSTR_LOCAL_GET:
                    console.log('Cmd:', '0x' + command.toString(16), "Param:", buffer.arr[pointer + 1], "Stack:", stack);
                    stack.push(params[buffer.arr[pointer + 1]]);
                    pointer += 2;
                    break;
                case INSTR_CALL:
                    console.log('Cmd:', '0x' + command.toString(16), "Param:", buffer.arr[pointer + 1], "Stack:", stack);
                    call(buffer.arr[pointer + 1], stack);
                    pointer += 2;
                    break;
                case INSTR_I32_ADD:
                    console.log('Cmd:', '0x' + command.toString(16), "Stack:", stack);
                    add(stack);
                    pointer += 1;
                    break;                    
            }
        }

        return stack.pop();
    }

    function getSection(index) {        
        function getTypeSection() {
            const sectionSize = decodeUInt32([buffer.arr[index + 1]]) + 2;
            const typesCnt = decodeUInt32([buffer.arr[index + 2]]);
            let secIndex = 0;
            const types = [];
            const startIndex = index + 3;

            while (secIndex < sectionSize && types.length < typesCnt) {
                const typeId = [buffer.arr[startIndex + secIndex]];
                console.log("Sec type id:", typeId.toString(16));
                const t = typeFunctions[typeId](buffer, startIndex + secIndex);
                types.push(t);
                secIndex += t.length;
            }

            return {
                type: 'SEC_TYPE',
                typeId: SEC_TYPE,
                size: sectionSize,
                types,
            }
        }

        function getFnSection() {
            const sectionSize = decodeUInt32([buffer.arr[index + 1]]) + 2;
            const fnsCount = decodeUInt32([buffer.arr[index + 2]]);
            const functions = [];
            for (let i=0; i<fnsCount; i++) {
                functions.push(buffer.arr[index + 3 + i]);
            }
            return {
                type: 'SEC_FUNC',
                typeId: SEC_FUNC,
                size: sectionSize,
                functions,
            }
        }

        function getCodeSection() {
            const sectionSize = decodeUInt32([buffer.arr[index + 1]]) + 2;
            const fnsCount = decodeUInt32([buffer.arr[index + 2]]);
            const blocks = [];
            let blockIndex = 0;

            for (let i=0; i<fnsCount; i++) {
                const blockSize = decodeUInt32([buffer.arr[index + 3 + blockIndex]]) + 1;
                const localsCount = decodeUInt32([buffer.arr[index + 3 + blockIndex + 1]]);
                const localsTypes = [];

                for (let i=0; i<localsCount; i++) {
                    localsTypes.push(buffer.arr[index + 3 + blockIndex + 2 + i])
                }

                const codePointer = index + 3 + blockIndex + 2 + localsCount;

                blocks.push({
                    size: blockSize,
                    localsCount,
                    localsTypes,
                    codePointer
                });
                blockIndex += blockSize;
            }

            return {
                type: 'SEC_CODE',
                typeId: SEC_CODE,
                size: sectionSize,
                blocks
            };
        }

        function getImportSection() {
            const sectionSize = decodeUInt32([buffer.arr[index + 1]]) + 2;
            const importsCount = decodeUInt32([buffer.arr[index + 2]]);
            const imports = [];
            let offset = 0;

            for (let i=0; i<importsCount; i++) {
                const moduleNameLength = decodeUInt32([buffer.arr[index + 3 + offset + 0]]);
                const moduleName = new TextDecoder().decode(
                    new Uint8Array(buffer.arr.slice(index + 3 + offset + 1, index + 3 + offset + 1 + moduleNameLength)));
                const fnNameLength = decodeUInt32([buffer.arr[index + 3 + offset + 1 + moduleNameLength]]);
                const fnName = new TextDecoder().decode(
                    new Uint8Array(buffer.arr.slice(index + 3 + offset + 2 + moduleNameLength, index + 3 + offset + 2 + moduleNameLength + fnNameLength)));
                const signatureIndex = buffer.arr[index + 3 + offset + 2 + moduleNameLength + fnNameLength + 1];
                const blockLength = 4 + moduleNameLength + fnNameLength;

                imports.push({
                    moduleName,
                    fnName,
                    signatureIndex
                });

                offset += blockLength;
            }

            return {
                size: sectionSize,
                type: 'SEC_IMPORT',
                typeId: SEC_IMPORT,
                imports,
            }
        }

        const secType = buffer.arr[index];
        console.log('Reading section:', '0x' + secType.toString(16));
        switch (secType) {
            case SEC_TYPE:
                return getTypeSection();
            case SEC_FUNC:
                return getFnSection();
            case SEC_CODE:
                return getCodeSection();
            case SEC_IMPORT:
                return getImportSection();
        }
        return null;
    }

    function startVm() {
        let pointer = 8;        

        while (pointer < buffer.length) {
            let section = getSection(pointer);
            if (section) {
                console.log('Found section:', section);
                sections.push(section);
                pointer += section.size;
            } else {
                console.warn('Cannot find next section in binary');
                break;
            }
        }

        // run start function
        console.log('Run first fn block:');
        const block = sections.find(f => f.typeId === SEC_CODE)?.blocks[0];
        if (block) {
            let fnResult = fun(block.codePointer, []);
            console.log('First function returned:', fnResult);
        } else {
            throw new Error("Couldn't find first function code");
        }
    }
    startVm();
    //console.log(fun(25, [30]));
}

function start() {
    const module = "AGFzbQEAAAABCgJgAAF/YAF/AX8DAwIAAQoXAgcAQYkGEAELDQEBf0EBIQEgACABagsAFARuYW1lAQYBAQNpbmMCBQIAAAEA";
    const bin = getBinary(module);

    fs.writeFile("binArray.txt", JSON.stringify(bin), () => {});

    const {magic, version} = getWasmMeta(bin);

    console.log('Buffer:', bin.arr.map((b, i) => `${i}: ${b.toString(16)}`), bin.length);

    const testMem = [0, 0, 0, 0, 0, 0, 0, 0];

    /*runvm(bin, {
        index: {
            log: (arg) => console.log('From VM:', arg),
            getMem: (i) => testMem[i],
            setMem: (i, v) => testMem[i] = v,
        }
    });*/

    //console.log('Test MEM:', testMem.map((m, i) => i + ': 0x' + m.toString(16)).join(', '));    
}

start();