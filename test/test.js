const ArchiveManager = require('../build/Release/manager');

/*ArchiveManager.ListContent('./test/test-zip.zip',function(err,files){
    console.log(files);
});*/
ArchiveManager.GetInfo('entry_11.txt','./test/test-tar.tar',function(err,info){
    if(err){
        console.error(err);
        return;
    }
    console.log(info.directory);
});