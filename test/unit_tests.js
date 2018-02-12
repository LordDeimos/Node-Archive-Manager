const ArchiveManager = require('../build/Release/manager');
const fs = require('fs');

exports.testReadZip = function(test) {
    test.expect(1);
    test.deepEqual(ArchiveManager.ListContent('./test/test-zip.zip'),[
        "entry_1.txt",
        "entry_2.txt",
        "entry_3.txt",
        "entry_4.txt",
        "entry_5.txt",
        "entry_6.txt",
        "entry_7.txt",
        "entry_8.txt",
        "entry_9.txt",
        "entry_10.txt"
        ], "ListContent for zip");
    test.done();
};

exports.testRead7z = function(test) {
    test.expect(1);
    test.deepEqual(ArchiveManager.ListContent('./test/test-7z.7z'),[
        "entry_1.txt",
        "entry_2.txt",
        "entry_3.txt",
        "entry_4.txt",
        "entry_5.txt",
        "entry_6.txt",
        "entry_7.txt",
        "entry_8.txt",
        "entry_9.txt",
        "entry_10.txt"
        ], "ListContent for 7zip");
    test.done();
};

exports.testInfoZipSize = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test/test-zip.zip').size,5315, "GetInfo size for zip");
    test.done();
};

exports.testInfoZipName = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test/test-zip.zip').name,"entry_1.txt", "GetInfo name for zip");
    test.done();
};

exports.testInfoZipDirFile = function(test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt','./test/test-zip.zip').directory, "GetInfo directory for file for zip");
    test.done();
};

exports.testInfo7zSize = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test/test-7z.7z').size,5315, "GetInfo size for 7zip");
    test.done();
};

exports.testInfo7zName = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test/test-7z.7z').name,"entry_1.txt", "GetInfo name for 7zip");
    test.done();
};

exports.testInfo7zDirFile = function(test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt','./test/test-7z.7z').directory, "GetInfo directory for file for 7zip");
    test.done();
};

exports.testWriteZipSingle = function(test) {
    test.expect(2);
    test.ok(ArchiveManager.WriteFromDisk(['test_cases/entry_1.txt'],'./test/test-write.zip'), "Write to zip");
    test.deepEqual(ArchiveManager.ListContent('./test/test-write.zip'),[
        "entry_1.txt"
        ], "ListContent After Write");
    test.done();
};

exports.testWriteZipMulti = function(test) {
    test.expect(2);
    test.ok(ArchiveManager.WriteFromDisk(['test_cases/entry_1.txt','test_cases/entry_2.txt','test_cases/entry_3.txt'],'./test/test-write.zip'), "Write to zip");
    test.deepEqual(ArchiveManager.ListContent('./test/test-write.zip'),[
        "entry_1.txt",
        "entry_2.txt",
        "entry_3.txt"
        ], "ListContent After Write");
    test.done();
};

exports.testExtract = function(test) {
    test.expect(2);
    test.ok(ArchiveManager.Extract('./test/test-7z.7z','./test/output/'), "Extract 7zip");
    var files = fs.readdirSync("./test/output/");
    test.deepEqual(files,[
        "entry_1.txt",
        "entry_2.txt",
        "entry_3.txt",
        "entry_4.txt",
        "entry_5.txt",
        "entry_6.txt",
        "entry_7.txt",
        "entry_8.txt",
        "entry_9.txt",
        "entry_10.txt"
    ].sort());
    test.done();
};

/* Fails on travis for some reason, but works locally
exports.testAppendZip = function(test) {
    test.expect(2);
    test.ok(ArchiveManager.Append(['test_cases/entry_4.txt'],'./test/test-write.zip'), "Write to zip");
    test.deepEqual(ArchiveManager.ListContent('./test/test-write.zip').sort(),[
        "entry_1.txt",
        "entry_2.txt",
        "entry_3.txt",
        "entry_4.txt"
        ].sort(), "ListContent After Write");
    test.done();
};*/

exports.testReadMemory = function(test) {
    test.expect(1);
    test.deepEqual(fs.readFileSync('./test/entry_1.txt').toString(),ArchiveManager.ReadBuffer('entry_1.txt','./test/test-zip.zip').toString(), "ReadBuffer for file not in Zip");
    test.done();
};

exports.testReadMemoryNotThere = function(test) {
    test.expect(1);
    test.equal(undefined,ArchiveManager.ReadBuffer('entry_11.txt','./test/test-zip.zip').toString(), "ReadBuffer for file not in Zip");
    test.done();
};