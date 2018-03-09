const test = require('tape');
const fs = require('fs');
const ArchiveManager = require('../build/Release/manager');

/**
 * Test Plan
 * 
 * Content
 *  -    Test the directory flag for a directory
 *  -     "    "     "       "    "  " file in a directory
 *  
 * Extract
 *  -   Folder structure is preserved
 * 
 * Read
 *  -   Data is preserved (text can be read)
 *  -   Json is parseable
 *  -   Exception when archive path is wrong
 *  -   Excpetion when wanted file name is wrong/not present
 *  -   Excepction when archive is corrupt
 * 
 * Append
 *  -   outcome is true
 *  -   Data is written
 *  -   Written data is not corrupt
 *  -   append one file
 *          -   Disk
 *          -   Buffer
 *  -   append multiple files
 *          -   Disk
 *          -   Buffers
 *  -   Exception is thrown when archive is not there
 *  -   Exception when archive corrupt
 *  -   Exception when data is corrupt
 *          -   Corrupt file
 *          -   Corrupt buffer
 * 
 * Remove
 *  -   File is removed
 *  -   outcome is true
 *  -   exception when file is not there
 *  -   exception when archive is not there
 *  -   exception when arhive is corrupt
 */

//Content

test('Content Returns Array', (t) => {
    t.plan(2);
    ArchiveManager.Content('./test/test-zip.zip', (error, files) => {
        t.error(error);
        t.ok(Array.isArray(files));
    });
});

test('Content Return Array Size', (t) => {
    t.plan(2);
    ArchiveManager.Content('./test/test-zip.zip', (error, files) => {
        t.error(error);
        t.equal(files.length,10);
    });
});

test('Content Return Array File Names', (t) => {
    t.plan(11);
    ArchiveManager.Content('./test/test-zip.zip', (error, files) => {
        t.error(error);
        var expected = [
            'entry_1.txt',
            'entry_2.txt',
            'entry_3.txt',
            'entry_4.txt',
            'entry_5.txt',
            'entry_6.txt',
            'entry_7.txt',
            'entry_8.txt',
            'entry_9.txt',
            'entry_10.txt',
        ];
        files.forEach((element) => {
            var index = expected.indexOf(element.name);
            t.notEqual(index,-1);
            expected.splice(index,1);
        });
    });
});

test('Content Return Array File Sizes', (t) => {
    t.plan(11);
    ArchiveManager.Content('./test/test-zip.zip', (error, files) => {
        t.error(error);
        files.forEach((element) => {
            t.equal(element.size,5315);
        });
    });
});

test('Content Return Array File Flags', (t) => {
    t.plan(11);
    ArchiveManager.Content('./test/test-zip.zip', (error, files) => {
        t.error(error);
        files.forEach((element) => {
            t.notOk(element.directory);
        });
    });
});

test('Content Exception Archive Not Present', (t) => {
    t.plan(1);
    ArchiveManager.Content('./test/test-not-here.zip', (error, files) => {
        if(error){
            t.pass("Error was thrown");
        }
    });
});

test('Content Exception Archive Corrupt', (t) => {
    t.plan(1);
    ArchiveManager.Content('./test/test-zip-corrupt.zip', (error, files) => {
        if(error){
            t.pass("Error was thrown");
        }
    });
});

test('Content Exception No archive Arg', (t) => {
    t.plan(1);
    try{
        ArchiveManager.Content((error, files) => {});
    }catch(exception){
        t.pass("Error was thrown")
    }
});

test('Content Exception Callback Missing', (t) => {
    t.plan(1);
    try{
        ArchiveManager.Content("./test/test-zip.zip");
    }catch(exception){
        t.pass("Error was thrown")
    }
});

//Create

test("Create Outcome",(t)=>{
    t.plan(2);
    ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        fs.unlinkSync('./test/test-create.zip');
    });
});

test("Create Exception Wrong Path",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Create(['./test/entry_1.txt'],'',(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
});

test("Create Integrity Names",(t)=>{
    t.plan(4);
    ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files[0].name,'entry_1.txt');//Becase there 'should' only be one
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Integrity Size",(t)=>{
    t.plan(4);
    ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files[0].size,5315);//Becase there 'should' only be one
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Integrity Number",(t)=>{
    t.plan(4);
    ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files.length,1);
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Outcome Multi",(t)=>{
    t.plan(2);
    ArchiveManager.Create(['./test/entry_1.txt','./test/entry_2.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        fs.unlinkSync('./test/test-create.zip');
    });
});

test("Create Integrity Names Multi",(t)=>{
    t.plan(5);
    ArchiveManager.Create(['./test/entry_1.txt','./test/entry_2.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files[0].name,'entry_1.txt');
            t.equals(files[1].name,'entry_2.txt');
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Integrity Size Multi",(t)=>{
    t.plan(5);
    ArchiveManager.Create(['./test/entry_1.txt','./test/entry_2.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files[0].size,5315);
            t.equals(files[1].size,5315);
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Integrity Number Multi",(t)=>{
    t.plan(4);
    ArchiveManager.Create(['./test/entry_1.txt','./test/entry_2.txt'],'./test/test-create.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content('./test/test-create.zip',(error,files)=>{
            t.error(error);
            t.equals(files.length,2);
            fs.unlinkSync('./test/test-create.zip');
        });
    });
});

test("Create Exception Incorrect Files",(t)=>{
    t.plan(1);
    ArchiveManager.Create(['./test/entry_11.txt'],'./test/test-create.zip',(error,outcome)=>{
        if(error){
            t.pass("Error was thrown");
            return;
        }
        t.fail("Error was not thrown");
        fs.unlinkSync('./test/test-create.zip');
    });
});

test("Create Exception Incorrect Type",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Create('./test/entry_1.txt','./test/test-create.zip',(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
});

test("Create Exception Incorrect Type Element",(t)=>{
    t.plan(1);
    ArchiveManager.Create([1],'./test/test-create.zip',(error,outcome)=>{
        if(error){
            t.pass("Error was thrown");
            return;
        }            
        fs.unlinkSync('./test/test-create.zip');
        t.fail("Error was not thrown");
    });
});

test("Create Exception undefined files",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Create(undefined,'./test/test-create.zip',(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
});

test("Create Exception undefined archive",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Create(['./test/entry_1.txt'],undefined,(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
});

/*test("Create Exception archive already exists",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-zip.zip',(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
}); I'm not running this test yet, because I know it will fail and I want prevent overwritting test-zip.zip*/

//Extract

test("Extract Outcome",(t)=>{
    t.plan(2);
    ArchiveManager.Extract('./test/test-zip.zip','./test/output/',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        fs.readdirSync('./test/output/').forEach((element)=>{
            fs.unlinkSync('./test/output/'+element);
        });
    });
});

test("Extract Outcome no output path",(t)=>{
    t.plan(2);
    ArchiveManager.Extract('./test/test-zip.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        [
            'entry_1.txt',
            'entry_2.txt',
            'entry_3.txt',
            'entry_4.txt',
            'entry_5.txt',
            'entry_6.txt',
            'entry_7.txt',
            'entry_8.txt',
            'entry_9.txt',
            'entry_10.txt',
        ].forEach((element)=>{
            fs.unlinkSync('./'+element);
        });
    });
});

test("Extract File Integrity Name",(t)=>{
    t.plan(11);
    ArchiveManager.Extract('./test/test-zip.zip','./test/output/',(error,outcome)=>{
        t.error(error);
        var expected = [
            'entry_1.txt',
            'entry_2.txt',
            'entry_3.txt',
            'entry_4.txt',
            'entry_5.txt',
            'entry_6.txt',
            'entry_7.txt',
            'entry_8.txt',
            'entry_9.txt',
            'entry_10.txt',
        ];
        var files = fs.readdirSync('./test/output/');
        files.forEach((element)=>{
            var index = expected.indexOf(element);
            t.notEqual(index,-1);
            expected.splice(index,1);
        });
        files.forEach((element)=>{
            fs.unlinkSync('./test/output/'+element);
        });
    });
});

test("Extract File Integrity Size",(t)=>{
    t.plan(11);
    ArchiveManager.Extract('./test/test-zip.zip','./test/output/',(error,outcome)=>{
        t.error(error);
        var files = fs.readdirSync('./test/output/');
        files.forEach((element)=>{
            t.equal(fs.statSync('./test/output/'+element).size,5315);
        });        
        files.forEach((element)=>{
            fs.unlinkSync('./test/output/'+element);
        });
    });
});

test("Extract Exception Corrupt",(t)=>{
    t.plan(1);
    ArchiveManager.Extract('./test/test-zip-corrupt.zip','./test/output/',(error,outcome)=>{
        if(error){
            t.pass("Error was thrown");
            return;
        }
        t.fail("Error was not thrown");
    });
});

/*test("Extract Exception Missing Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Extract('','./test/output/',(error,outcome)=>{
        if(error){
            t.pass("Error was thrown");
            return;
        }
        t.fail("Error was not thrown");
    });
}); This creates an infinite loop*/

test("Extract Exception Missing with actuall name",(t)=>{
    t.plan(1);
    ArchiveManager.Extract('./test/test.zip','./test/output/',(error,outcome)=>{
        if(error){
            t.pass("Error was thrown");
            return;
        }
        t.fail("Error was not thrown");
    });
});

//Read

//Append

//Remove