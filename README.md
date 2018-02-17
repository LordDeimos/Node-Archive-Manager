# Node-Archive-Manager
[![Build Status](https://travis-ci.org/LordDeimos/Node-Archive-Manager.svg?branch=master)](https://travis-ci.org/LordDeimos/Node-Archive-Manager)
[![Build status](https://ci.appveyor.com/api/projects/status/9p5qxv6vqd539iyo?svg=true)](https://ci.appveyor.com/project/LordDeimos/node-archive-manager)
[![NPM Version](https://img.shields.io/npm/v/archive-manager.svg)](https://www.npmjs.com/package/archive-manager)
[![License](https://img.shields.io/github/license/LordDeimos/Node-Archive-Manager.svg)](https://github.com/LordDeimos/Node-Archive-Manager/blob/master/LICENSE)

A Node.js plugin to manipulate archive files using C++

## Installation
As per usual, use your node package manager of choice to install this in your project:
```sh
npm install archive-manager
```
This will require node-gyp to be present, if you don't have it:
```sh
npm install -g node-gyp
```
## Usage
Here are some basic use cases:

Require:
```js
const ArchiveManager = require('archive-manager');
```

ListContent:
```js
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

In-Place edits are currently not supported by libarchive, so this will be very slow for large archives
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
This uses node-gyp to build

### For Windows
The libarchive package and its dependencies are provied for both 64 and 32 bit windows, so all you need to do is:
```powershell
npm install
node-gyp rebuild
```
### For Linux
For linux distributions, install the libarchive development package using your chosen packge manager, eg:
```sh
sudo apt-get install libarchive-dev
```

Then
```sh
npm install
node-gyp rebuild
```