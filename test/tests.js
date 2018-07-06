const test = require('tape');
const fs = require('fs');
const ArchiveManager = require('../build/Release/manager');

var testError = (t,error)=>{
    if(error){
        t.pass("Error was thrown");
        return;
    }
    t.fail("Error was not thrown");
}

/**
 * Test Plan
 * 
 * Content
 *  -    Test the directory flag for a directory
 *  -     "    "     "       "    "  " file in a directory
 *  
 * Extract
 *  -   Output is not corrupt
 *  -   Folder structure is preserved
 * 
 * Read
 *  -   Excepction when archive is corrupt
 * 
 * Append
 *  -   append multiple files
 *          -   Disk
 *          -   Buffers
 *  -   Exception when data is corrupt can I test for this though?
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
            "entry_10.txt"
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
        testError(t,error);
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
            fs.unlinkSync('./test/test-create.zip');
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
            fs.unlinkSync('./test/test-create.zip');     
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
    //fs.unlinkSync('./test/test-create.zip');
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
    //fs.unlinkSync('./test/test-create.zip');
});

test("Create Exception archive already exists",(t)=>{
    t.plan(1);
    ArchiveManager.Create(['./test/entry_1.txt'],'./test/test-zip.zip',(error,outcome)=>{
        testError(t,error);
    });
});

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
        testError(t,error);
    });
});

/*test("Extract Exception Missing Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Extract('','./test/output/',(error,outcome)=>{
        testError(t,error);
    });
}); This creates an infinite loop*/

test("Extract Exception Missing with actuall name",(t)=>{
    t.plan(1);
    ArchiveManager.Extract('./test/test.zip','./test/output/',(error,outcome)=>{
        testError(t,error);
    });
});

//Read

test("Read Text Is preserved",(t)=>{
    t.plan(2);
    ArchiveManager.Read("entry_1.txt","./test/test-zip.zip",(error,data)=>{
        t.error(error);
        var buffer = fs.readFileSync("./test/entry_1.txt");
        t.equal(buffer.toString(),data.toString());
    });
});

/*test("Read JSON Is preserved",(t)=>{
    t.plan(2);
    ArchiveManager.Read("entry_1.txt","./test/test-zip.zip",(error,data)=>{
        t.error(error);
        var buffer = fs.readFileSync("./test/entry_1.txt");
        t.equal(buffer.toString(),data.toString());
    });
});Need a file with a json in it first*/

test("Read Exception Wrong Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Read("entry_1.txt","not_there.zip",(error,data)=>{
        testError(t,error);
    });
});

test("Read Missing Internal File",(t)=>{
    t.plan(1);
    ArchiveManager.Read("not_there.txt","test-zip.txt",(error,data)=>{
        testError(t,error);
    });
});

//Append

 test("Append Outcome is True",(t)=>{
    t.plan(2);
    ArchiveManager.Append(["./test/entry_2.txt"],"./test/test-append.zip",(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
    });
 });

 test("Append Data is Written",(t)=>{
    t.plan(4);
    ArchiveManager.Append(["./test/entry_3.txt"],"./test/test-append.zip",(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content("./test/test-append.zip",(error,files)=>{
            t.error(error);
            t.equal(files.length,3);
        });
    });
 });

 test("Append Data is not Corrupt",(t)=>{
    t.plan(4);
    ArchiveManager.Append(["./test/entry_4.txt"],"./test/test-append.zip",(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        var buffer = fs.readFileSync("./test/entry_4.txt");
        ArchiveManager.Read("entry_4.txt","./test/test-append.zip",(error,data)=>{
            t.error(error);
            t.equal(data.toString(),buffer.toString());
        });
    });
 });

 test("Append Data from Buffer",(t)=>{
    t.plan(4);    
    var buffer = fs.readFileSync("./test/entry_5.txt");
    ArchiveManager.Append(['entry_5.txt'],[buffer],'./test/test-append.zip',(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
        ArchiveManager.Content("./test/test-append.zip",(error,files)=>{
            t.error(error);
            t.equal(files.length,5);
        });
    });
 });

test("Append Exception Missing Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Append(['./test/entry_1.txt'],'./test/not-here.zip',(error,outcome)=>{
        testError(t,error);
    });
});

test("Append Corrupt Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Append(["./test/extry_1.zip"],"./test/test-zip-corrupt.zip",(error,outcome)=>{
        testError(t,error);
    });
});

test("Append Existing File",(t)=>{
    t.plan(1);
    ArchiveManager.Append(["./test/entry_1.txt"],"./test/test-append.zip",(error,outcome)=>{
        testError(t,error);
    });
});

test("Append Wrong Arg Type",(t)=>{
    t.plan(1);
    try{
        ArchiveManager.Append('./test/entry_1.txt','./test/test-append.zip',(error,outcome)=>{});
        t.fail("Error was not thrown");
    }
    catch(exception){
        t.pass("Error was thrown");
    }
});

//Remove

test("Remove Outcome",(t)=>{
    t.plan(2);
    ArchiveManager.Remove(["entry_2.txt"],"./test/test-remove.zip",(error,outcome)=>{
        t.error(error);
        t.ok(outcome);
    });
});

test("Remove File is Removed",(t)=>{
    t.plan(3);
    ArchiveManager.Remove(["entry_3.txt"],"./test/test-remove.zip",(error,outcome)=>{
        t.error(error);
        ArchiveManager.Content("./test/test-remove.zip",(error,files)=>{
            t.equal(files.length,1);
            t.equal(files[0].name,"entry_1.txt");
        });
    });
});

test("Remove File is Missing",(t)=>{
    t.plan(3);
    ArchiveManager.Remove(["entry_3.txt"],"./test/test-remove.zip",(error,outcome)=>{
        t.error(error);
        ArchiveManager.Content("./test/test-remove.zip",(error,files)=>{
            t.equal(files.length,1);
            t.equal(files[0].name,"entry_1.txt");
        });
    });
});

test("Remove Archive is Missing",(t)=>{
    t.plan(1);
    ArchiveManager.Remove(["entry_3.txt"],"./test/test-not-there.zip",(error,outcome)=>{
        testError(t,error);
    });
});

test("Remove Corrupt Archive",(t)=>{
    t.plan(1);
    ArchiveManager.Remove(["entry_3.txt"],"./test/test-corrupt.zip",(error,outcome)=>{
        testError(t,error);
    });
});