const ArchiveManager = require('./build/Debug/archive_manager');

console.log(ArchiveManager.ListContent('./test7.cb7'));
console.log(ArchiveManager.GetInfo('ComicInfo.xml','./test7.cb7'));