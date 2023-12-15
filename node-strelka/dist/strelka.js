/*
 * ATTENTION: The "eval" devtool has been used (maybe by default in mode: "development").
 * This devtool is neither made for production nor for readable output files.
 * It uses "eval()" calls to create a separate source file in the browser devtools.
 * If you are trying to read the output file, select a different devtool (https://webpack.js.org/configuration/devtool/)
 * or disable the default devtool with "devtool: false".
 * If you are looking for production-ready output files, see mode: "production" (https://webpack.js.org/configuration/mode/).
 */
/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "./src/fb/drawer.ts":
/*!**************************!*\
  !*** ./src/fb/drawer.ts ***!
  \**************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";
eval("\nObject.defineProperty(exports, \"__esModule\", ({ value: true }));\nexports.Drawer = void 0;\nconst pureimage_1 = __webpack_require__(/*! pureimage */ \"./node_modules/pureimage/dist/browser.js\");\nconst COLOR_DEFAULT = '#000000';\nconst FONT_DEFAULT = '12 monospace';\nclass Drawer {\n    constructor(width, height) {\n        this.width = width;\n        this.height = height;\n        this.bitmap = (0, pureimage_1.make)(width, height);\n        this.ctx = this.bitmap.getContext('2d');\n    }\n    clear(color) {\n        this.ctx.fillStyle = color || COLOR_DEFAULT;\n        this.ctx.fillRect(0, 0, this.width, this.height);\n        this.ctx.fillStyle = COLOR_DEFAULT;\n    }\n    text(text, x, y, fontSize, color) {\n        this.ctx.fillStyle = color;\n        this.ctx.font = `${fontSize} monospace`;\n        this.ctx.fillText(text, x, y);\n        this.ctx.font = FONT_DEFAULT;\n        this.ctx.fillStyle = COLOR_DEFAULT;\n    }\n    toBuffer() {\n        const { data } = this.bitmap;\n        const size = this.width * this.height;\n        const fbArray = new Uint8Array(size * 4);\n        for (let i = 0; i < size; i++) {\n            fbArray[i * 4 + 0] = data[i * 4 + 2];\n            fbArray[i * 4 + 1] = data[i * 4 + 1];\n            fbArray[i * 4 + 2] = data[i * 4 + 0];\n            fbArray[i * 4 + 3] = 0;\n        }\n        return fbArray;\n    }\n    get context() {\n        return this.ctx;\n    }\n}\nexports.Drawer = Drawer;\n\n\n//# sourceURL=webpack://strelka/./src/fb/drawer.ts?");

/***/ }),

/***/ "./src/fb/framebuffer.ts":
/*!*******************************!*\
  !*** ./src/fb/framebuffer.ts ***!
  \*******************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";
eval("\nvar __importDefault = (this && this.__importDefault) || function (mod) {\n    return (mod && mod.__esModule) ? mod : { \"default\": mod };\n};\nObject.defineProperty(exports, \"__esModule\", ({ value: true }));\nexports.writeToFb = exports.getFbResolution = void 0;\nconst fs_1 = __importDefault(__webpack_require__(/*! fs */ \"?4712\"));\nconst logger_1 = __webpack_require__(/*! ../logger */ \"./src/logger.ts\");\nconst FB_RESOLUTION_PATH = '/sys/class/graphics/fb0/virtual_size';\nconst FB_DEV_PATH = '/dev/fb0';\nfunction getFbResolution() {\n    const resolution = fs_1.default.readFileSync(FB_RESOLUTION_PATH);\n    if (!resolution) {\n        throw new Error('Can not determine fb resolution');\n    }\n    const [fbWidth, fbHeight] = resolution.toString().split(',').map(v => +v);\n    (0, logger_1.log0)('Found resolution:', `${fbWidth}x${fbHeight}`);\n    return [fbWidth, fbHeight];\n}\nexports.getFbResolution = getFbResolution;\nfunction writeToFb(buffer) {\n    const fb = fs_1.default.openSync(FB_DEV_PATH, 'w+');\n    const dv = new DataView(buffer.buffer);\n    fs_1.default.writeSync(fb, dv, 0, dv.byteLength, 0);\n}\nexports.writeToFb = writeToFb;\n\n\n//# sourceURL=webpack://strelka/./src/fb/framebuffer.ts?");

/***/ }),

/***/ "./src/logger.ts":
/*!***********************!*\
  !*** ./src/logger.ts ***!
  \***********************/
/***/ ((__unused_webpack_module, exports) => {

"use strict";
eval("\nObject.defineProperty(exports, \"__esModule\", ({ value: true }));\nexports.log0 = exports.log = void 0;\nfunction log(logLevel, ...args) {\n    if (!process.env.LOG_LEVEL || logLevel >= parseInt(process.env.LOG_LEVEL)) {\n        console.log('LOG_' + logLevel + ' :: ', ...args);\n    }\n}\nexports.log = log;\nfunction log0(...args) {\n    log(0, ...args);\n}\nexports.log0 = log0;\n\n\n//# sourceURL=webpack://strelka/./src/logger.ts?");

/***/ }),

/***/ "./src/main.ts":
/*!*********************!*\
  !*** ./src/main.ts ***!
  \*********************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";
eval("\n//framebuffer example\nObject.defineProperty(exports, \"__esModule\", ({ value: true }));\nconst drawer_1 = __webpack_require__(/*! ./fb/drawer */ \"./src/fb/drawer.ts\");\nconst framebuffer_1 = __webpack_require__(/*! ./fb/framebuffer */ \"./src/fb/framebuffer.ts\");\nfunction main() {\n    const [fbWidth, fbHeight] = (0, framebuffer_1.getFbResolution)();\n    const fbDrawer = new drawer_1.Drawer(fbWidth, fbHeight);\n    fbDrawer.clear('#ee67f5');\n    fbDrawer.text(\"S T R E L K A\", fbWidth / 2 - 100, fbHeight / 2 - 20, 38, '#ffffff');\n    const fb = fbDrawer.toBuffer();\n    (0, framebuffer_1.writeToFb)(fb);\n}\nmain();\n\n\n//# sourceURL=webpack://strelka/./src/main.ts?");

/***/ }),

/***/ "?4712":
/*!********************!*\
  !*** fs (ignored) ***!
  \********************/
/***/ (() => {

eval("/* (ignored) */\n\n//# sourceURL=webpack://strelka/fs_(ignored)?");

/***/ }),

/***/ "./node_modules/pureimage/dist/browser.js":
/*!************************************************!*\
  !*** ./node_modules/pureimage/dist/browser.js ***!
  \************************************************/
/***/ ((__unused_webpack___webpack_module__, __webpack_exports__, __webpack_require__) => {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nconsole.log(\"we are in the browser. No need to do anything. Just use new Canvas()\");\nexports.make = function (width, height) {\n    let canvas = document.createElement(\"canvas\");\n    canvas.width = width;\n    canvas.height = height;\n    return canvas;\n};\n//# sourceMappingURL=browser.js.map\n\n//# sourceURL=webpack://strelka/./node_modules/pureimage/dist/browser.js?");

/***/ })

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
/******/ 	/* webpack/runtime/make namespace object */
/******/ 	(() => {
/******/ 		// define __esModule on exports
/******/ 		__webpack_require__.r = (exports) => {
/******/ 			if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 				Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 			}
/******/ 			Object.defineProperty(exports, '__esModule', { value: true });
/******/ 		};
/******/ 	})();
/******/ 	
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module can't be inlined because the eval devtool is used.
/******/ 	var __webpack_exports__ = __webpack_require__("./src/main.ts");
/******/ 	
/******/ })()
;