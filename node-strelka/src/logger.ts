export function log(logLevel: number, ...args: string[]) {
    if (!process.env.LOG_LEVEL || logLevel >= parseInt(process.env.LOG_LEVEL)) {
        console.log('LOG_' + logLevel + ' :: ', ...args);
    }
}

export function log0(...args: any[]) {
    log(0, ...args);
}