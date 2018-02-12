const ArchiveManager = require('./build/Release/archive_manager');

//console.log(ArchiveManager.ReadBuffer('entry_1.txt','./test_cases/test-zip.zip').toString('base64'));
console.log(ArchiveManager.ReadBuffer('entry_1.txt','./test_cases/test-zip.zip').toString());