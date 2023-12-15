//framebuffer example

import { Drawer } from "./fb/drawer";
import { getFbResolution, writeToFb } from "./fb/framebuffer";

function main() {
    const [fbWidth, fbHeight] = getFbResolution();
    const fbDrawer = new Drawer(fbWidth, fbHeight);

    fbDrawer.clear('#ee67f5');
    fbDrawer.text("S T R E L K A", 20, 100, 20, '#ffffff');
    
    const fb = fbDrawer.toBuffer();
    writeToFb(fb);
}

main();