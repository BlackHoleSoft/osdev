//framebuffer example
const fs = require('fs');

function main() {
    const fb = fs.openSync('/dev/fb0', 'w');
    let buffer = new Uint8Array([255, 60, 60, 0]).buffer;
    fs.writeSync(fb, buffer, 0, buffer.byteLength, 0);
}

main();