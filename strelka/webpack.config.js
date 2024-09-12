const path = require('path');

module.exports = {
    entry: './src/index.js',
    mode: 'production',    
    output: {
        filename: 'entry.js',
        path: path.resolve(__dirname, 'dist'),
    },
};
