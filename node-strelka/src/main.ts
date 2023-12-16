//framebuffer example

import { StrelkaConfig } from "./config";
import { Drawer } from "./fb/drawer";
import { getFbResolution, writeToFb } from "./fb/framebuffer";
import { KDMODE, setGraphicsMode } from "./ioctl";

function main() {
    StrelkaConfig.loadConfig();

    if (StrelkaConfig.config?.graphicsMode) {
        setGraphicsMode(KDMODE.GRAPHICS);
    }

    const [fbWidth, fbHeight] = getFbResolution();
    const fbDrawer = new Drawer(fbWidth, fbHeight);

    fbDrawer.clear('#aa67b5');
    fbDrawer.text("S T R E L K A", fbWidth / 2 - 110, fbHeight / 2 - 20, 40, '#ffffff');
    
    const fb = fbDrawer.toBuffer();
    writeToFb(fb);

    setTimeout(() => {
        if (StrelkaConfig.config?.graphicsMode) {
            setGraphicsMode(KDMODE.TEXT);
        }
    }, 5000);
}

main();