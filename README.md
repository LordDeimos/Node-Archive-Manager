# Node-Archive-Manager
[![Build Status](https://travis-ci.org/LordDeimos/Node-Archive-Manager.svg?branch=master)](https://travis-ci.org/LordDeimos/Node-Archive-Manager)

A Node.js plugin to manipulate archive files using C++

## Installation
This currently is only for linux, as Windows has issues with libarchive which I am trying to resolve.

As per usual, use your node package manager of choice to install this in your project:
```sh
npm install archive-manager
```

## Usage
Here are some basic use cases:

ListContent:
```js
const ArchiveManager = require('archive-manager');

var content = ArchiveManager.ListContent('path/to/archive');

```

GetInfo:
```js
var info = ArchiveManager.GetInfo('internal/file','path/to/archive');
```

WriteFromDisk:
```js
if(ArchiveManager.WriteFromDisk(['path/to/file/1','path/to/file/2','path/to/file/3'],'path/to/archive') === false){
    console.error("Failed")
}
```

Append:
```js
if(ArchiveManager.Append('path/to/file','path/to/archive') === false){
    console.error("Failed")
}
```

Extract:
```js
if(ArchiveManager.Extract('path/to/archive','path/to/output') === false){
    console.error("Failed")
}
```

ReadBuffer:
```js
var buffer = ArchiveManager.ReadBuffer('internal/path','path/to/archive');
```

## Building From Source
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