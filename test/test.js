const ArchiveManager = require('../build/Release/manager');
const fs = require('fs');

var testObj = {
    title:"Test"
}

var buffer = Buffer.from(JSON.stringify(testObj));

var string2 = "Some Sample Text with more chars123456789";
var buffer2 = Buffer.from(string2);

var buffer3 = fs.readFileSync('./test/test_image.png');

ArchiveManager.Create(['info.json','test.txt','test_image.png'],[Buffer.from(JSON.stringify(testObj)),buffer2,buffer3],'./test/test-write.zip',function(err,outcome){
    if(err){
        console.log(err);
    }
    var testObj2 = {
        prop:"Hello!"
    }
    var buffer4 = fs.readFileSync('./test/test_image2.png');
    
    ArchiveManager.Append(['new.json','test_image2.png'],[Buffer.from(JSON.stringify(testObj2)),buffer4],'./test/test-write.zip',function(err,outcome){
        if(err){
            console.log(err);
        }
    });
});