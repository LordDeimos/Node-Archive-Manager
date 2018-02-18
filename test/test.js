const ArchiveManager = require('../build/Release/manager');

ArchiveManager.Extract('./test/test-zip.zip', './test/output/',function(err,outcome){
    if(err){
        console.error(err);
        return;
    }
    console.log(outcome);
});