import { RenderBuffer } from './buffer';

export class Renderer {
    buffer: RenderBuffer;

    constructor(width: number, height: number) {
        this.buffer = new RenderBuffer(width, height);
    }
}
