import {make, Bitmap, encodePNGToStream, registerFont} from 'pureimage';
import { StrelkaConfig } from '../config';
import fs from 'fs';
import { log0 } from '../logger';

const FONT_FAMILY_DEFAULT = "'exo-medium'";
const COLOR_DEFAULT = '#000000';
const FONT_DEFAULT = `12 ${FONT_FAMILY_DEFAULT}`;

export class Drawer {

    private width: number;
    private height: number;    
    private bitmap: Bitmap;
    private ctx;

    constructor(width: number, height: number) {
        this.width = width;
        this.height = height;

        registerFont('./fonts/Exo2-Medium.ttf', 'exo-medium').loadSync();

        this.bitmap = make(width, height);
        this.ctx = this.bitmap.getContext('2d');

    }

    clear(color?: string) {
        this.ctx.fillStyle = color || COLOR_DEFAULT;
        this.ctx.fillRect(0, 0, this.width, this.height);
        this.ctx.fillStyle = COLOR_DEFAULT;
    }

    text(text: string, x: number, y: number, fontSize: number, color: string) {
        this.ctx.fillStyle = color;
        this.ctx.font = `${fontSize} ${FONT_FAMILY_DEFAULT}`;
        this.ctx.fillText(text, x, y);
        this.ctx.font = FONT_DEFAULT;
        this.ctx.fillStyle = COLOR_DEFAULT;
    }

    toPng() {
        log0('Debug drawer to fb.png');
        encodePNGToStream(this.bitmap, fs.createWriteStream("fb.png"))
        .then(() => {
            console.log("FB snapshot was saved to ./fb.png");
        })
        .catch((e) => {
            console.log("Error while encoding FB snapshot to stream");
        });
    }

    toBuffer() {
        const {data} = this.bitmap;
        const size = this.width * this.height;
        const fbArray = new Uint8Array(size * 4);

        for (let i = 0; i < size; i++) {
            fbArray[i * 4 + 0] = data[i * 4 + 2];
            fbArray[i * 4 + 1] = data[i * 4 + 1];
            fbArray[i * 4 + 2] = data[i * 4 + 0];
            fbArray[i * 4 + 3] = 0;
        }

        if (StrelkaConfig.config?.drawerDebug) {
            this.toPng();
        }

        return fbArray;
    }

    get context() {
        return this.ctx;
    }
}