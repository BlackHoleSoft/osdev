const Codegen = require('./codegen');
const fs = require('fs');

const args = process.argv;
const filename = args[2];

const initCode = fs.readFileSync('./init.js', 'utf8');
const code = initCode + '\n' + fs.readFileSync(filename, 'utf8');


const codegen = new Codegen();
const bytecode = codegen.generate(code);

fs.writeFileSync('./program.bin', Buffer.from(bytecode));