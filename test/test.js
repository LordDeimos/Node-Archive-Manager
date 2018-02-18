const ArchiveManager = require('../build/Release/manager');

ArchiveManager.ListContent('./test/test-zip.zip',function(err,files){
    console.log(files);
});
//console.log(ArchiveManager.ReadBuffer('page007.jpeg','O:\\Comics\\Noragami volume 1.cbz').toString('base64'));