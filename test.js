const ArchiveManager = require('./build/Debug/archive_manager');
const fs = require('fs');

fs.readdir("./test_cases/output/",function(err,files){
    if(err){
        console.error(err);
    }
    files.sort();
    console.log(files);
});