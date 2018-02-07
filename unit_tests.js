const ArchiveManager = require('./build/Release/archive_manager');

exports.testReadZip = function(test) {
    test.expect(1);
    test.deepEqual(ArchiveManager.ListContent('./test_cases/test-zip.zip'),[
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
    test.deepEqual(ArchiveManager.ListContent('./test_cases/test-7z.7z'),[
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
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-zip.zip').size,5315, "GetInfo size for zip");
    test.done();
};

exports.testInfoZipName = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-zip.zip').name,"entry_1.txt", "GetInfo name for zip");
    test.done();
};

exports.testInfoZipDirFile = function(test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-zip.zip').directory, "GetInfo directory for file for zip");
    test.done();
};

exports.testInfo7zSize = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-7z.7z').size,5315, "GetInfo size for 7zip");
    test.done();
};

exports.testInfo7zName = function(test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-7z.7z').name,"entry_1.txt", "GetInfo name for 7zip");
    test.done();
};

exports.testInfo7zDirFile = function(test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt','./test_cases/test-7z.7z').directory, "GetInfo directory for file for 7zip");
    test.done();
};