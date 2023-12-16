const path = require('path');
const CopyPlugin = require("copy-webpack-plugin");

module.exports = {
  entry: './src/main.ts',
  mode: 'development',
  target: 'node',
  plugins: [
    new CopyPlugin({
      patterns: [
        { from: path.resolve(__dirname, 'strelka.config.json'), to: path.resolve(__dirname, 'dist/strelka.config.json') },
        { from: path.resolve(__dirname, 'win.strelka.config.json'), to: path.resolve(__dirname, 'dist/win.strelka.config.json') },
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