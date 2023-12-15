"use strict";
//framebuffer example
Object.defineProperty(exports, "__esModule", { value: true });
const drawer_1 = require("./fb/drawer");
const framebuffer_1 = require("./fb/framebuffer");
function main() {
    const [fbWidth, fbHeight] = (0, framebuffer_1.getFbResolution)();
    const fbDrawer = new drawer_1.Drawer(fbWidth, fbHeight);
    fbDrawer.clear('#ee67f5');
    fbDrawer.text("S T R E L K A", fbWidth / 2 - 100, fbHeight / 2 - 20, 38, '#ffffff');
    const fb = fbDrawer.toBuffer();
    (0, framebuffer_1.writeToFb)(fb);
}
main();
