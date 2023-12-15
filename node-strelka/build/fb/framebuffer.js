"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.writeToFb = exports.getFbResolution = void 0;
const fs_1 = __importDefault(require("fs"));
const logger_1 = require("../logger");
const FB_RESOLUTION_PATH = '/sys/class/graphics/fb0/virtual_size';
const FB_DEV_PATH = '/dev/fb0';
function getFbResolution() {
    const resolution = fs_1.default.readFileSync(FB_RESOLUTION_PATH);
    if (!resolution) {
        throw new Error('Can not determine fb resolution');
    }
    const [fbWidth, fbHeight] = resolution.toString().split(',').map(v => +v);
    (0, logger_1.log0)('Found resolution:', `${fbWidth}x${fbHeight}`);
    return [fbWidth, fbHeight];
}
exports.getFbResolution = getFbResolution;
function writeToFb(buffer) {
    const fb = fs_1.default.openSync(FB_DEV_PATH, 'w+');
    const dv = new DataView(buffer.buffer);
    fs_1.default.writeSync(fb, dv, 0, dv.byteLength, 0);
}
exports.writeToFb = writeToFb;
