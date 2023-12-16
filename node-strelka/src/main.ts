//framebuffer example

import { StrelkaConfig } from "./config";
import { Drawer } from "./fb/drawer";
import { getFbResolution, writeToFb } from "./fb/framebuffer";

function main() {
    StrelkaConfig.loadConfig();

    const [fbWidth, fbHeight] = getFbResolution();
    const fbDrawer = new Drawer(fbWidth, fbHeight);

    fbDrawer.clear('#aa67b5');
    fbDrawer.text("S T R E L K A", fbWidth / 2 - 110, fbHeight / 2 - 20, 40, '#ffffff');
    
    const fb = fbDrawer.toBuffer();
    writeToFb(fb);
}

main();