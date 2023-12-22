import fs from 'fs';
import { log0 } from './logger';

export type StrelkaConfigType = {
    fbDevice: string;
    fbResolutionFile: string;
    drawerDebug?: boolean;
    graphicsMode?: boolean;
    fbResolution?: string;
}

export class StrelkaConfig {
    static config: StrelkaConfigType | null = null;

    static loadConfig() {
        const platform = process.platform || 'linux';
        const strelkaConfig = platform === 'win32' ? './win.strelka.config.json' : './strelka.config.json';

        log0('Load config from:', strelkaConfig);        
        const conf = fs.readFileSync(strelkaConfig);
        if (!conf) {
            throw new Error("Coundn't find config file from path: ./strelka.config.json");
        }

        StrelkaConfig.config = JSON.parse(conf.toString()) as StrelkaConfigType;
        log0(StrelkaConfig.config);
    }
}