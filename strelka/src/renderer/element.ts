export type Outline = [number, number, number, number];
export type Color = [number, number, number, number];
export type Position = [number, number];
export type Direction = 'horizontal' | 'vertical';

export interface RenderableElement {
    children: RenderableElement[];
    width?: number;
    height?: number;
    padding: Outline;
    margin: Outline;
    border: Outline;
    innerText?: string;
    color: Color;
    borderColor: Color;
    backgroundColor: Color;

    render(parentPosition: Position, direction: Direction): void;
}
