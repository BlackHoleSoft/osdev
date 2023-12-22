//framebuffer example

import { StrelkaConfig } from "./config";
import { Drawer } from "./fb/drawer";
import { getFbResolution, writeToFb } from "./fb/framebuffer";
import { KDMODE, setGraphicsMode } from "./ioctl";
import { log0 } from "./logger";
import { ComponentProps, component, renderComponents } from "./screens/component";
import { startScreen } from "./screens/startScreen";
import { state } from "./screens/state";
import { use } from "./screens/use";

function main() {
    StrelkaConfig.loadConfig();

    if (StrelkaConfig.config?.graphicsMode) {
        setGraphicsMode(KDMODE.GRAPHICS);
    }

    const [fbWidth, fbHeight] = getFbResolution();
    const fbDrawer = new Drawer(fbWidth, fbHeight);

    const rootComponent = ({context}: ComponentProps) => {
        log0('Start root component');

        const [mode, setMode] = state(KDMODE.GRAPHICS);

        // use(() => {
        //     if (StrelkaConfig.config?.graphicsMode) {
        //         setGraphicsMode(mode);
        //     } else {
        //         log0('Switch graphics mode to:', mode);
        //     }
        // }, [mode]);

        // use(() => {
        //     setTimeout(() => setMode(KDMODE.TEXT), 5000);
        // }, []);

        return () => {
            if (mode === 0x1) {
                component('startScreen', startScreen, {context, timeout: 3000});
            }
        }
    }

    renderComponents({
        drawer: fbDrawer
    }, rootComponent);

    // fbDrawer.clear('#aa67b5');
    // fbDrawer.text("S T R E L K A", fbWidth / 2 - 110, fbHeight / 2 - 20, 40, '#ffffff');
    
    // const fb = fbDrawer.toBuffer();
    // writeToFb(fb);

    setTimeout(() => {
        if (StrelkaConfig.config?.graphicsMode) {
            setGraphicsMode(KDMODE.TEXT);
        }
    }, 5000);

    // while (true) {
    //     // final loop
    // }
}

main();