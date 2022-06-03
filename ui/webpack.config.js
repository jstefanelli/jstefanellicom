const html = require('html-webpack-plugin');
const path = require('path');
const webpack = require('webpack');

module.exports = (env, args) => {
    const dev = args.mode != 'production';

    /** @type {webpack.Configuration} */
    const config = {
        entry: './src/main.ts',
        output: {
            path: path.resolve(__dirname, 'build'),
            filename: 'main.js',
            clean: true
        },
        mode: dev ? 'development' : 'production',
        devtool: dev ? 'inline-cheap-source-map' : false,
        plugins: [
            new html({
                template: './src/index.template.html'
            })
        ],
        stats: 'minimal',
        resolve: {
            extensions: [ '.js', '.ts' ]
        },
        module: {
            rules: [
                {
                    test: /.tsx?$/,
                    use: [ 'ts-loader' ],
                    exclude: /node_modules/
                },
                {
                    test: /.css$/,
                    use: [ 'style-loader', 'css-loader', 'postcss-loader' ]
                },
                {
                    test: /.(eot|svg|ttf|woff|woff2|png|jpg|gig)$/i,
                    type: 'asset'
                }
            ]
        }
    }

    return config;
}