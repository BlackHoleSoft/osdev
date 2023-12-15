//framebuffer example
import fs from 'fs';

function main() {
    const fb = fs.openSync('/dev/fb0', 'w+');

    const resolution = fs.readFileSync('/sys/class/graphics/fb0/virtual_size');
    if (!resolution) {
        throw new Error('Can not determine fb resolution');
    }
    const [fbWidth, fbHeight] = resolution.toString().split(',').map(v => +v);
    console.log('Resolution:', `${fbWidth}x${fbHeight}`);

    const fbArr = new Uint8Array(fbWidth*fbHeight*4);

    for (let i=0; i < fbWidth * fbHeight; i+=3) {
        fbArr[i * 4 + 0] = 0; // blue
        fbArr[i * 4 + 1] = 0; // green
        fbArr[i * 4 + 2] = 255; // red
        fbArr[i * 4 + 3] = 0;
    }

    const buffer = new DataView(new Uint8Array(fbArr).buffer);
    fs.writeSync(fb, buffer, 0, buffer.byteLength, 0);

    
}

main();