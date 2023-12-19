import { writeToFb } from "../fb/framebuffer";
import { ComponentProps } from "./component";

export type StartScreenProps = {
    timeout: number;
} & ComponentProps;

export function startScreen({context, timeout}: StartScreenProps) {

    const {drawer} = context;
    const [fbWidth, fbHeight] = drawer.resolution;

    return () => {
        drawer.clear('#aa67b5');
        drawer.text("S T R E L K A", fbWidth / 2 - 110, fbHeight / 2 - 20, 40, '#ffffff');
        
        const fb = drawer.toBuffer();
        writeToFb(fb);
    }
}