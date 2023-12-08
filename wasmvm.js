console.log('Run test wasm vm');

// consts
const INSTR_I32_CONST = 0x41;
const INSTR_I32_ADD = 0x6a;
const INSTR_END = 0x0b;
const INSTR_LOCAL_GET = 0x20;
const INSTR_FUNC = 0x60;

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
    /*value |= 0;
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
    }*/
    return [value];
};

function encodeUInt32(value) {    
    return [value];
};

function decodeSInt32(input) {
    /*let result = 0;
    let shift = 0;
    while (true) {
      const byte = input.shift();
      result |= (byte & 0x7f) << shift;
      shift += 7;
      if ((0x80 & byte) === 0) {
        if (shift < 32 && (byte & 0x40) !== 0) {
          return result | (~0 << shift);
        }
        return result;
      }
    }*/
    return input[0];
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
    const retType = buffer.arr[offset + 2 + paramsCnt + 1];
    return {
        params,
        retType,
        length: 2 + paramsCnt + 1 + 1/*returned cnt*/
    };
}

function runvm(buffer) {
    console.log('Run vm...');    

    function add(stack) {
        let res = decodeSInt32([stack.pop()]) + decodeSInt32([stack.pop()]);
        stack.push(encodeSInt32(res));
    }

    function fun(addr, params) {
        const stack = [];
        let pointer = addr;

        while (pointer < buffer.length && buffer.arr[pointer] != INSTR_END) {
            const command = buffer.arr[pointer];

            console.log('Cmd:', '0x' + command.toString(16), "Stack:", stack);

            switch (command) {
                case INSTR_I32_CONST:
                    stack.push(buffer.arr[pointer + 1]);
                    pointer += 2
                    break;
                case INSTR_LOCAL_GET:
                    stack.push(params[buffer.arr[pointer + 1]]);
                    pointer += 2;
                    break;
                case INSTR_I32_ADD:
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
                size: sectionSize,
                functions,
            }
        }

        const secType = buffer.arr[index];
        console.log('Reading section:', '0x' + secType.toString(16));
        switch (secType) {
            case SEC_TYPE:
                return getTypeSection();
            case SEC_FUNC:
                return getFnSection();
        }
        return null;
    }
    
    function findStartFun() {
    
    }

    function startVm() {
        let pointer = 8;

        const sections = [];

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
    }
    startVm();
    //console.log(fun(25, [30]));
}

function start() {
    const module = "AGFzbQEAAAABBQFgAAF/AwIBAAoKAQgAQcAAQQVqCwAKBG5hbWUCAwEAAA==";
    const bin = getBinary(module);

    const {magic, version} = getWasmMeta(bin);

    console.log('Buffer:', bin.arr.map((b, i) => `${i}: ${b.toString(16)}`), bin.length);

    runvm(bin);
}

start();