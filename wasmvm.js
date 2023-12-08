console.log('Run test wasm vm');

// consts
const INSTR_I32_CONST = 0x41;
const INSTR_I32_ADD = 0x6a;
const INSTR_END = 0x0b;
const INSTR_LOCAL_GET = 0x20;
const INSTR_FUNC = 0x60;

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

function encodeSignedLeb128FromInt32(value) {
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

function decodeSignedLeb128(input) {
    let result = 0;
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
    }
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

function runvm(buffer) {
    console.log('Run vm...');
    console.log('Buffer:', buffer.arr.map((b, i) => `${i}: ${b.toString(16)}`), buffer.length);

    function add(stack) {
        let res = decodeSignedLeb128([stack.pop()]) + decodeSignedLeb128([stack.pop()]);
        stack.push(encodeSignedLeb128FromInt32(res));
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

    console.log(fun(25, [30]));
}

function start() {
    const module = "AGFzbQEAAAABBgFgAX8BfwMCAQAKCQEHAEEHIABqCwAKBG5hbWUCAwEAAA==";
    const bin = getBinary(module);

    const {magic, version} = getWasmMeta(bin);

    runvm(bin);
}

start();