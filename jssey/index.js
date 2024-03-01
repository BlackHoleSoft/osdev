const Codegen = require('./codegen');

const codegen = new Codegen();
codegen.generate('let a = 7 + 2, b = 9; let test = a + b; test = a + 10;');