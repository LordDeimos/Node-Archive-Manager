const ArchiveManager = require('../build/Release/manager');

console.log(ArchiveManager.ListContent('./test/test-xz.tar.xz',function(){
    console.log("Dummy");
}).toString());
//console.log(ArchiveManager.ReadBuffer('page007.jpeg','O:\\Comics\\Noragami volume 1.cbz').toString('base64'));