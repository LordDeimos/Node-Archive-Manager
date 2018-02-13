const ArchiveManager = require('../build/Release/manager');

//console.log(ArchiveManager.ReadBuffer('entry_1.txt','./test_cases/test-zip.zip').toString('base64'));
console.log(ArchiveManager.ReadBuffer('page007.jpeg','/media/alex/Comics/Comics/Noragami volume 1.cbz').toString('base64'));