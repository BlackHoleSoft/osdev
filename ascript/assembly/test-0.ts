// The entry file of your WebAssembly module.
export declare function log(i: i32): void;
export declare function getMem(i: i32): i32;
export declare function setMem(i: i32, value: i32): void;

export function _start(): i32 {
  let c_h = 0x48;
  let c_e = 0x65;
  let c_l = 0x6c;

  printchar(c_h, 20, 0xa);
  printchar(c_e, 22, 0xa);
  printchar(c_l, 24, 0xa);
  printchar(c_l, 26, 0xa);

  return 0;
}

function printchar(code: i32, pos: i32, color: i32): void {
  let start: i32 = 0xb8000;
  let offset: i32 = pos * 2;

  setMem(start + offset + 0, code);
  setMem(start + offset + 1, color);
}
