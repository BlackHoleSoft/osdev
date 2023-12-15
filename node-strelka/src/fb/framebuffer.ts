import fs from 'fs';
import { log0 } from '../logger';

const FB_RESOLUTION_PATH = '/sys/class/graphics/fb0/virtual_size';
const FB_DEV_PATH = '/dev/fb0';

export function getFbResolution() {
    const resolution = fs.readFileSync(FB_RESOLUTION_PATH);
    if (!resolution) {
        throw new Error('Can not determine fb resolution');
    }
    const [fbWidth, fbHeight] = resolution.toString().split(',').map(v => +v);
    log0('Found resolution:', `${fbWidth}x${fbHeight}`);
    return [fbWidth, fbHeight];
}

export function writeToFb(buffer: Uint8Array) {
    const fb = fs.openSync(FB_DEV_PATH, 'w+');    

    const dv = new DataView(buffer.buffer);
    fs.writeSync(fb, dv, 0, dv.byteLength, 0);
}