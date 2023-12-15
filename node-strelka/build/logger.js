"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.log0 = exports.log = void 0;
function log(logLevel, ...args) {
    if (!process.env.LOG_LEVEL || logLevel >= parseInt(process.env.LOG_LEVEL)) {
        console.log('LOG_' + logLevel + ' :: ', ...args);
    }
}
exports.log = log;
function log0(...args) {
    log(0, ...args);
}
exports.log0 = log0;
