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

exports.testInfoZip = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-zip.zip', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for zip");
        }
        test.equal(info.size, 5315, "GetInfo size for zip");
        test.ok(!info.directory, "GetInfo type for zip");
        test.done();
    });
};

exports.testWriteZipSingle = function (test) {
    test.expect(3);
    ArchiveManager.WriteFromDisk(['test/entry_1.txt'], './test/test-write.zip', function (err, outcome) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
        ArchiveManager.ListContent('./test/test-write.zip', function (err, files) {
            if (err) {
                console.error(err);
                test.done();
                return;
            }
            test.deepEqual(files, [
                "entry_1.txt"
            ], "ListContent After Write");
            ArchiveManager.GetInfo('entry_1.txt','./test/test-write.zip',function(err,info){
                if(err){
                    console.log(err);
                    test.done();
                    return;
                }
                test.equal(info.size,5315,"GetInfo size after Write");
                test.done();
            });
        });
    });
};

exports.testWriteZipMulti = function (test) {
    test.expect(3);
    ArchiveManager.WriteFromDisk(['test/entry_1.txt', 'test/entry_2.txt', 'test/entry_3.txt'], './test/test-write.zip', function (err, outcome) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
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
            ArchiveManager.GetInfo('entry_1.txt','./test/test-write.zip',function(err,info){
                if(err){
                    console.log(err);
                    test.done();
                    return;
                }
                test.equal(info.size,5315,"GetInfo size after Write");
                test.done();
            });
        });
    });
};

exports.testExtract = function (test) {
    test.expect(2);
    ArchiveManager.Extract('./test/test-zip.zip', './test/output/',function(err,outcome){
        if(err){
            console.error(err);
            test.done();
            return;
        }
        test.ok(outcome, "Extract 7zip");
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
    });
};

/* Fails on travis for some reason, but works locally*/
exports.testAppendZip = function(test) {
    test.expect(2);
    ArchiveManager.Append(['test/entry_4.txt'],'./test/test-write.zip', function(err,outcome){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
        ArchiveManager.ListContent('./test/test-write.zip',function(err,files){
            if(err){
                console.error(err);
                test.done();
                return;
            }
            test.deepEqual(files.sort(),[
                "entry_1.txt",
                "entry_2.txt",
                "entry_3.txt",
                "entry_4.txt"
                ].sort(), "ListContent After Write");
            test.done();
        });
    });
};

exports.testReadMemory = function (test) {
    test.expect(1);
    ArchiveManager.ReadBuffer('entry_1.txt', './test/test-zip.zip',function(err,data){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.deepEqual(fs.readFileSync('./test/entry_1.txt').toString(), data.toString(), "ReadBuffer for file in Zip");
        test.done();
    });
};

exports.testReadMemoryNotThere = function (test) {
    test.expect(1);
    ArchiveManager.ReadBuffer('entry_11.txt', './test/test-zip.zip', function(err,data){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.equal(data, undefined, "ReadBuffer for file not in Zip");
        test.done();
    });
};

//7z
exports.testRead7z = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-7z.7z', function (err, files) {
        if (err) {
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

exports.testInfo7z = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-7z.7z', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for 7z");
        }
        test.equal(info.size, 5315, "GetInfo size for 7z");
        test.ok(!info.directory, "GetInfo type for 7z");
        test.done();
    });
};

//tar
exports.testReadTar = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-tar.tar', function (err, files) {
        if (err) {
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

exports.testInfotar = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-tar.tar', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for tar");
        }
        test.equal(info.size, 5315, "GetInfo size for tar");
        test.ok(!info.directory, "GetInfo type for tar");
        test.done();
    });
};

//Bzip
exports.testReadbz2 = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-bz2.tar.bz2', function (err, files) {
        if (err) {
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

exports.testInfobz2 = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-bz2.tar.bz2', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for bz2");
        }
        test.equal(info.size, 5315, "GetInfo size for bz2");
        test.ok(!info.directory, "GetInfo type for bz2");
        test.done();
    });
};

//Gzip
exports.testReadgz = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-gz.tar.gz', function (err, files) {
        if (err) {
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

exports.testInfogz = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-gz.tar.gz', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for gz");
        }
        test.equal(info.size, 5315, "GetInfo size for gz");
        test.ok(!info.directory, "GetInfo type for gz");
        test.done();
    });
};

//Xz
exports.testReadxz = function (test) {
    test.expect(1);
    ArchiveManager.ListContent('./test/test-xz.tar.xz', function (err, files) {
        if (err) {
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

exports.testInfoxz = function (test) {
    if (process.platform !== 'linux') {
        test.expect(3);
    } else {
        test.expect(2);
    }
    ArchiveManager.GetInfo('entry_1.txt', './test/test-xz.tar.xz', function (err, info) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        if (process.platform !== 'linux') {
            test.equal(info.name, "entry_1.txt", "GetInfo name for xz");
        }
        test.equal(info.size, 5315, "GetInfo size for xz");
        test.ok(!info.directory, "GetInfo type for xz");
        test.done();
    });
};