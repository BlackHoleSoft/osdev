import fs from 'fs';
import { log0 } from './logger';

export type StrelkaConfigType = {
    fbDevice: string;
    fbResolutionFile: string;
    drawerDebug?: boolean;
    graphicsMode?: boolean;
}

export class StrelkaConfig {
    static config: StrelkaConfigType | null = null;

    static loadConfig() {
        log0('Load config from:', './strelka.config.json');        
        const conf = fs.readFileSync('./strelka.config.json');
        if (!conf) {
            throw new Error("Coundn't find config file from path: ./strelka.config.json");
        }

        StrelkaConfig.config = JSON.parse(conf.toString()) as StrelkaConfigType;
        log0(StrelkaConfig.config);
    }
}