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
Here is a basic use case:

```js
const ArchiveManager = require('archive-manager');

ArchiveManager.Content('path/to/archive',function(error,files){
    if(error){
        console.error(error);
        return;
    }
    files.forEach(function(file){
        ArchiveManager.GetInfo(file,'path/to/archive',function(error,info){
            if(error){
                console.error(error);
                return;
            }
            console.log(info);
        });
    });
});
```
### API
There are six functions in this module:
```js
Content()
GetInfo()
Create()
Append()
Extract()
Read()
```
For more detail view the [wiki](https://github.com/LordDeimos/Node-Archive-Manager/wiki/API).

In-Place edits are currently not supported by libarchive, so this will be very slow for large archives

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