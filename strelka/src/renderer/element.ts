import { RenderBuffer } from './buffer';

export type Outline = [number, number, number, number];
export type Color = [number, number, number, number];
export type Position = [number, number];
export type Direction = 'horizontal' | 'vertical';
export type Rect = {
    x: number;
    y: number;
    width: number;
    height: number;
};

type RenderableProps = {
    width?: number;
    height?: number;
    padding: Outline;
    margin: Outline;
    border: Outline;
    innerText?: string;
    color: Color;
    borderColor: Color;
    backgroundColor: Color;
};

export interface RenderableElement {
    children: RenderableElement[];
    props: RenderableProps;

    render(parentRect: Rect, direction: Direction): Rect;
}

export class BoxElement implements RenderableElement {
    children: RenderableElement[];
    props: RenderableProps;
    buffer: RenderBuffer;
    rect: Rect;

    constructor(buffer: RenderBuffer, props: RenderableProps, children?: RenderableElement[]) {
        this.children = children || [];
        this.props = props;
        this.buffer = buffer;

        this.rect = {
            x: 0,
            y: 0,
            height: 0,
            width: 0,
        };
    }

    private calcRect = (parentRect: Rect, direction: Direction) => {};

    private preDrawText = (text: string) => {};

    render = (parentRect: Rect, direction: Direction) => {
        // pre draw innerText to tmp buffer
        // calc rect based on text dimensions
        // draw box using element dimensions
        // draw innerText

        return this.rect;
    };
}
