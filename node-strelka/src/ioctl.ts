import {exec} from 'node:child_process';

export enum KDMODE {
    TEXT = 0,
    GRAPHICS = 1
}

export function setGraphicsMode(mode: KDMODE) {
    exec(`ioctl /dev/console 0x4B3A	-v ${mode}`);
}