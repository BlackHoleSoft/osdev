// clone https://github.com/jerome-pouiller/ioctl
// exclude some ioctls from sh script, which generates errors during compilation
// run make && make install

import {exec} from 'node:child_process';
import { log0 } from './logger';

export enum KDMODE {
    TEXT = 0,
    GRAPHICS = 1
}

export function setGraphicsMode(mode: KDMODE) {
    log0("Switch graphics mode:", mode);
    exec(`ioctl /dev/console 0x4B3A	-v ${mode}`);
}