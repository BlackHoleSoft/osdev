type BufferColor = [r: number, g: number, b: number, a: number];

export class RenderBuffer {
    buffer: BufferColor[];
    width: number;
    height: number;

    constructor(width: number, height: number) {
        this.buffer = Array.apply(
            null,
            new Array(width * height).map(() => [0, 0, 0, 0]),
        );
    }

    getColor = (x: number, y: number) => {
        if (x < 0 || x >= this.width || y < 0 || y >= this.height) {
            throw new Error('Buffer position is out of bounds');
        }

        return this.buffer[x + y * this.width];
    };

    setColor = (x: number, y: number, color: BufferColor) => {
        if (x < 0 || x >= this.width || y < 0 || y >= this.height) {
            throw new Error('Buffer position is out of bounds');
        }

        this.buffer[x + y * this.width] = color;
    };
}
