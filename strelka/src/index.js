const { createDomObject } = require("./dom");

// clear viewport
process.stdout.write('\x1Bc');

console.log('Strelka Core running');
createDomObject();

window.test();

while(1) {}