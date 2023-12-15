"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Drawer = void 0;
const pureimage_1 = require("pureimage");
const COLOR_DEFAULT = '#000000';
const FONT_DEFAULT = '12 monospace';
class Drawer {
    constructor(width, height) {
        this.width = width;
        this.height = height;
        this.bitmap = (0, pureimage_1.make)(width, height);
        this.ctx = this.bitmap.getContext('2d');
    }
    clear(color) {
        this.ctx.fillStyle = color || COLOR_DEFAULT;
        this.ctx.fillRect(0, 0, this.width, this.height);
        this.ctx.fillStyle = COLOR_DEFAULT;
    }
    text(text, x, y, fontSize, color) {
        this.ctx.fillStyle = color;
        this.ctx.font = `${fontSize} monospace`;
        this.ctx.fillText(text, x, y);
        this.ctx.font = FONT_DEFAULT;
        this.ctx.fillStyle = COLOR_DEFAULT;
    }
    toBuffer() {
        const { data } = this.bitmap;
        const size = this.width * this.height;
        const fbArray = new Uint8Array(size * 4);
        for (let i = 0; i < size; i++) {
            fbArray[i * 4 + 0] = data[i * 4 + 2];
            fbArray[i * 4 + 1] = data[i * 4 + 1];
            fbArray[i * 4 + 2] = data[i * 4 + 0];
            fbArray[i * 4 + 3] = 0;
        }
        return fbArray;
    }
    get context() {
        return this.ctx;
    }
}
exports.Drawer = Drawer;
