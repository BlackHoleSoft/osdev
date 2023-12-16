const path = require('path');
const CopyPlugin = require("copy-webpack-plugin");

const platform = process.platform || 'linux';
const strelkaConfig = platform === 'win32' ? 'win.strelka.config.json' : 'strelka.config.json';

module.exports = {
  entry: './src/main.ts',
  mode: 'development',
  target: 'node',
  plugins: [
    new CopyPlugin({
      patterns: [
        { from: path.resolve(__dirname, strelkaConfig), to: path.resolve(__dirname, 'dist/strelka.config.json') },
        { from: path.resolve(__dirname, 'fonts'), to: path.resolve(__dirname, 'dist/fonts') }
      ],
    }),
  ],
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
    ],
  },
  resolve: {
    extensions: ['.tsx', '.ts', '.js'],
    fallback: {
      fs: false
    }
  },
  output: {
    filename: 'strelka.js',
    path: path.resolve(__dirname, 'dist'),
  },
};