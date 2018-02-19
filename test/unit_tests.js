const ArchiveManager = require('../build/Release/manager');
const fs = require('fs');
const os = require('os');

function sortData(a,b){
    var nameA = a.name.toLowerCase();
    var nameB = b.name.toLowerCase();
    if (nameA < nameB) {
        return -1;
    } else if (nameB < nameA) {
        return 1;
    }
    return 0;
}

//Zip
exports.testReadZip = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-zip.zip', function (err, files) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};

exports.testWriteZipSingle = function (test) {
    test.expect(4);
    ArchiveManager.Create(['test/entry_1.txt'], './test/test-write.zip', function (err, outcome) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
        ArchiveManager.Content('./test/test-write.zip', function (err, files) {
            if (err) {
                console.error(err);
                test.done();
                return;
            }
            var standard = [
                "entry_1.txt"
            ].sort();
            var i=0;
            files.sort(sortData).forEach(function(entry){
                test.equal(entry.name, standard[i], "GetInfo name after write");
                test.equal(entry.size, 5315, "GetInfo size after write");
                test.ok(!entry.directory, "GetInfo type after write");
                ++i;
            });
            test.done();
        });
    });
};

exports.testWriteZipMulti = function (test) {
    test.expect(10);
    ArchiveManager.Create(['test/entry_1.txt', 'test/entry_2.txt', 'test/entry_3.txt'], './test/test-write.zip', function (err, outcome) {
        if (err) {
            console.error(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
        ArchiveManager.Content('./test/test-write.zip', function (err, files) {
            if (err) {
                console.error(err);
                test.done();
                return;
            }
            var standard = [
                "entry_1.txt",
                "entry_2.txt",
                "entry_3.txt"
            ].sort();
            var i=0;
            files.sort(sortData).forEach(function(entry){
                test.equal(entry.name, standard[i], "GetInfo name for zip");
                test.equal(entry.size, 5315, "GetInfo size for zip");
                test.ok(!entry.directory, "GetInfo type for zip");
                ++i;
            });
            test.done();
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

exports.testAppendZip = function(test) {
    test.expect(13);
    ArchiveManager.Append(['test/entry_4.txt'],'./test/test-write.zip', function(err,outcome){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.ok(outcome, "Write to zip");
        ArchiveManager.Content('./test/test-write.zip',function(err,files){
            if(err){
                console.error(err);
                test.done();
                return;
            }
            var standard = [
                "entry_1.txt",
                "entry_2.txt",
                "entry_3.txt",
                "entry_4.txt"
            ].sort();
            var i=0;
            files.sort(sortData).forEach(function(entry){
                test.equal(entry.name, standard[i], "GetInfo name for zip");
                test.equal(entry.size, 5315, "GetInfo size for zip");
                test.ok(!entry.directory, "GetInfo type for zip");
                ++i;
            });
            test.done();
        });
    });
};

exports.testReadMemory = function (test) {
    test.expect(1);
    ArchiveManager.Read('entry_1.txt', './test/test-zip.zip',function(err,data){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.deepEqual(fs.readFileSync('./test/entry_1.txt').toString(), data.toString(), "Read for file in Zip");
        test.done();
    });
};

exports.testReadMemoryNotThere = function (test) {
    test.expect(1);
    ArchiveManager.Read('entry_11.txt', './test/test-zip.zip', function(err,data){
        if(err){
            console.log(err);
            test.done();
            return;
        }
        test.equal(data, undefined, "Read for file not in Zip");
        test.done();
    });
};

//7z
exports.testRead7z = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-7z.7z', function (err, files) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};

//tar
exports.testReadTar = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-tar.tar', function (err, files) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};

//Bzip
exports.testReadbz2 = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-bz2.tar.bz2', function (err, files) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};

//Gzip
exports.testReadgz = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-gz.tar.gz', function (err, files) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};

//Xz
exports.testReadxz = function (test) {
    test.expect(30);
    ArchiveManager.Content('./test/test-xz.tar.xz', function (err, files) {
        if (err) {
            console.log(err);
            test.done();
            return;
        }
        var standard = [
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
        ].sort();
        var i=0;
        files.sort(sortData).forEach(function(entry){
            test.equal(entry.name, standard[i], "GetInfo name for zip");
            test.equal(entry.size, 5315, "GetInfo size for zip");
            test.ok(!entry.directory, "GetInfo type for zip");
            ++i;
        });
        test.done();
    });
};