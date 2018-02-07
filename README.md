# Node-Archive-Manager
[![Build Status](https://travis-ci.org/LordDeimos/Node-Archive-Manager.svg?branch=master)](https://travis-ci.org/LordDeimos/Node-Archive-Manager)

A Node.js plugin to manipulate archive files using C++

## Building
This requires libarchive and node-gyp.

For linux distributions, install the libarchive development package using your chosen packge manager, eg:
```sh
sudo apt-get install libarchive-dev
```
To install node-gyp:
```sh
npm install -g node-gyp
```

Then
```sh
npm install
node-gyp configure build
```