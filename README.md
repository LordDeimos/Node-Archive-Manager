# Node-Archive-Manager
[![Build Status](https://travis-ci.org/LordDeimos/Node-Archive-Manager.svg?branch=master)](https://travis-ci.org/LordDeimos/Node-Archive-Manager)
[![Build status](https://ci.appveyor.com/api/projects/status/9p5qxv6vqd539iyo?svg=true)](https://ci.appveyor.com/project/LordDeimos/node-archive-manager)
[![NPM Version](https://img.shields.io/npm/v/archive-manager.svg)](https://www.npmjs.com/package/archive-manager)
[![License](https://img.shields.io/github/license/LordDeimos/Node-Archive-Manager.svg)](https://github.com/LordDeimos/Node-Archive-Manager/blob/master/LICENSE)

A Node.js plugin to manipulate archive files using C++ and [libarchive](https://www.libarchive.org/)

## Installation
As per usual, use your node package manager of choice to install this in your project:
```sh
npm install archive-manager
```
*There are some quirks to installing node-gyp, there are very required for windows*

You can get node-gyp [here](https://github.com/nodejs/node-gyp).

## Usage
Here is a basic use case:

```js
const ArchiveManager = require('archive-manager');

ArchiveManager.Content('path/to/archive', function(error, files){
    if(error){
        console.error(error);
        return;
    }
    files.forEach(function(file){
        console.log(file.name);
    });
});
```
### API
There are six functions in this module:
```js
Content()
Create()
Append()
Extract()
Read()
Remove()
```
For more detail view the [wiki](https://github.com/LordDeimos/Node-Archive-Manager/wiki/API)

## Building From Source

### For Windows
The libarchive package and its dependencies are provied for both 64 and 32 bit windows, but you may need to grab some extra things for node-gyp (see above):
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