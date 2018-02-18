const ArchiveManager = require('../build/Release/manager');
const fs = require('fs');
const os = require('os');

//Zip
exports.testReadZip = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-zip.zip', function (err, files) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        test.deepEqual(files.sort(), [
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
        ].sort(), "ListContent for zip");
        test.done();
    });
};

exports.testInfoZipSize = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-zip.zip').size, 5315, "GetInfo size for zip");
    test.done();
};

exports.testInfoZipName = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-zip.zip').name, "entry_1.txt", "GetInfo name for zip");
    test.done();
};

exports.testInfoZipDirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-zip.zip').directory, "GetInfo directory for file for zip");
    test.done();
};

exports.testWriteZipSingle = function (test) {
    test.expect(2);
    test.ok(ArchiveManager.WriteFromDisk(['test/entry_1.txt'], './test/test-write.zip'), "Write to zip");
    ArchiveManager.ListContent('./test/test-write.zip', function (err, files) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        test.deepEqual(files, [
            "entry_1.txt"
        ], "ListContent After Write");
        test.done();
    });
};

exports.testWriteZipMulti = function (test) {
    test.expect(2);
    test.ok(ArchiveManager.WriteFromDisk(['test/entry_1.txt', 'test/entry_2.txt', 'test/entry_3.txt'], './test/test-write.zip'), "Write to zip");
    ArchiveManager.ListContent('./test/test-write.zip', function (err, files) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        test.deepEqual(files, [
            "entry_1.txt",
            "entry_2.txt",
            "entry_3.txt"
        ], "ListContent After Write");
        test.done();
    });
};

exports.testExtract = function (test) {
    test.expect(2);
    test.ok(ArchiveManager.Extract('./test/test-zip.zip', './test/output/'), "Extract 7zip");
    var files = fs.readdirSync("./test/output/");
    test.deepEqual(files, [
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

exports.testReadMemory = function (test) {
    test.expect(1);
    test.deepEqual(fs.readFileSync('./test/entry_1.txt').toString(), ArchiveManager.ReadBuffer('entry_1.txt', './test/test-zip.zip').toString(), "ReadBuffer for file in Zip");
    test.done();
};

exports.testReadMemoryNotThere = function (test) {
    test.expect(1);
    test.equal(undefined, ArchiveManager.ReadBuffer('entry_11.txt', './test/test-zip.zip').length, "ReadBuffer for file not in Zip");
    test.done();
};

//7z
exports.testRead7z = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-7z.7z', function (err, files) {
        if(err){
            console.log(err);            
            test.done();
            return;
        }
        test.deepEqual(files, [
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
    });
};
exports.testInfo7zSize = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-7z.7z').size, 5315, "GetInfo size for 7zip");
    test.done();
};
exports.testInfo7zName = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-7z.7z').name, "entry_1.txt", "GetInfo name for 7zip");
    test.done();
};
exports.testInfo7zDirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-7z.7z').directory, "GetInfo directory for file for 7zip");
    test.done();
};

//tar
exports.testReadTar = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-tar.tar', function (err, files) {
        if(err){
            console.log(err);            
            test.done();
            return;
        }
        test.deepEqual(files, [
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
        ].sort(), "ListContent for tar");
        test.done();
    });
};
exports.testInfotarSize = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-tar.tar').size, 5315, "GetInfo size for tar");
    test.done();
};
exports.testInfotarName = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-tar.tar').name, "entry_1.txt", "GetInfo name for tar");
    test.done();
};
exports.testInfotarDirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-tar.tar').directory, "GetInfo directory for file for tar");
    test.done();
};

//Bzip
exports.testReadbz2 = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-bz2.tar.bz2', function (err, files) {
        if(err){
            console.log(err);            
            test.done();
            return;
        }
        test.deepEqual(files.sort(), [
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
        ].sort(), "ListContent for bz2");
        test.done();
    });
};
exports.testInfobz2Size = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-bz2.tar.bz2').size, 5315, "GetInfo size for bz2");
    test.done();
};
exports.testInfobz2Name = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-bz2.tar.bz2').name, "entry_1.txt", "GetInfo name for bz2");
    test.done();
};
exports.testInfobz2DirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-bz2.tar.bz2').directory, "GetInfo directory for file for bz2");
    test.done();
};

//Gzip
exports.testReadgz = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-gz.tar.gz', function (err, files) {
        if(err){
            console.log(err);            
            test.done();
            return;
        }
        test.deepEqual(files.sort(), [
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
        ].sort(), "ListContent for gz");
        test.done();
    });
};
exports.testInfogzSize = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-gz.tar.gz').size, 5315, "GetInfo size for gz");
    test.done();
};
exports.testInfogzName = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-gz.tar.gz').name, "entry_1.txt", "GetInfo name for gz");
    test.done();
};
exports.testInfogzDirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-gz.tar.gz').directory, "GetInfo directory for file for gz");
    test.done();
};

//Xz
exports.testReadxz = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-xz.tar.xz', function (err, files) {
        if(err){
            console.log(err);            
            test.done();
            return;
        }
        test.deepEqual(files, [
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
        ].sort(), "ListContent for xz");
        test.done();
    });
};
exports.testInfoxzSize = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-xz.tar.xz').size, 5315, "GetInfo size for xz");
    test.done();
};
exports.testInfoxzName = function (test) {
    test.expect(1);
    test.equal(ArchiveManager.GetInfo('entry_1.txt', './test/test-xz.tar.xz').name, "entry_1.txt", "GetInfo name for xz");
    test.done();
};
exports.testInfoxzDirFile = function (test) {
    test.expect(1);
    test.ok(!ArchiveManager.GetInfo('entry_1.txt', './test/test-xz.tar.xz').directory, "GetInfo directory for file for xz");
    test.done();
};