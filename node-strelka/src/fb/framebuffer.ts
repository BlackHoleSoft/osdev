import fs from 'fs';
import { log0 } from '../logger';
import { StrelkaConfig } from '../config';

export function getFbResolution() {
    const {fbResolutionFile} = StrelkaConfig.config!;

    const resolution = fs.readFileSync(fbResolutionFile);
    if (!resolution) {
        throw new Error('Can not determine fb resolution');
    }
    const [fbWidth, fbHeight] = resolution.toString().split(',').map(v => +v);
    log0('Found resolution:', `${fbWidth}x${fbHeight}`);
    return [fbWidth, fbHeight];
}

export function writeToFb(buffer: Uint8Array) {
    const {fbDevice} = StrelkaConfig.config!;
    const fb = fs.openSync(fbDevice, 'w+');

    const dv = new DataView(buffer.buffer);
    fs.writeSync(fb, dv, 0, dv.byteLength, 0);
}