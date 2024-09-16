import React from 'react';
import {render} from 'react-dom';

console.log('Entry');

const init = () => {
    const root = document.getElementById('root');
    render(<div>STRELKA</div>, root);
};

init();
