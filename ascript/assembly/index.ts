// The entry file of your WebAssembly module.
export declare function log(i: i32): void;
export declare function getMem(i: i32): i32;
export declare function setMem(i: i32, value: i32): void;

export function _start(): i32 {
  const i = 5;  
  writeDeadBeef(2);
  const sum = add(getMem(2), 1);
  log(sum);
  setMem(0, sum);
  return 0;
}

function add(a: i32, b: i32): i32 {
  return a + b;
}

function writeDeadBeef(start: i32): void {
  setMem(start + 0, 0xde);
  setMem(start + 1, 0xad);
  setMem(start + 2, 0xbe);
  setMem(start + 3, 0xef);
}
