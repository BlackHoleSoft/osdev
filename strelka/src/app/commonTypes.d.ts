declare module '*.module.scss';

declare namespace NodeJS {
    export interface ProcessEnv {
        NODE_ENV: 'development' | 'production' | 'test';
    }
}

declare module '*.png';
declare module '*.jpg';

declare module '*.svg' {
    import React = require('react');
    export const ReactComponent: React.FC<React.SVGProps<SVGSVGElement>>;
}
declare module 'img/svg/*.svg' {
    const id: string;
    const viewBox: string;
    const url: string;
    const toString: () => string;
}
