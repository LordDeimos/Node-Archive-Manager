const ArchiveManager = require('../build/Release/manager');

console.log(ArchiveManager.ReadBuffer('entry_1.txt','./test/test-7z.7z').toString());
//console.log(ArchiveManager.ReadBuffer('page007.jpeg','O:\\Comics\\Noragami volume 1.cbz').toString('base64'));