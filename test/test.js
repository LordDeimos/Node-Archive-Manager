const ArchiveManager = require('../build/Debug/manager');
const fs = require('fs');

var testObj = {
    title:"Test"
}

var buffer = Buffer.from(JSON.stringify(testObj));

ArchiveManager.Append(['info.json'],[Buffer.from(JSON.stringify(testObj))],
    '/media/alex/Comics/Comics/DC Comics/DC Comics - Bombshells Annual (2016 Annual)/DC Comics - Bombshells Annual Vol.2016 01.cb7',function(err,outcome){
    if(err){
        console.log(err);
        return;
    }
    console.log("Success");
    ArchiveManager.Read('DC Comics - Bombshells (2016-) Annual 001-000.jpg','/media/alex/Comics/Comics/DC Comics/DC Comics - Bombshells Annual (2016 Annual)/DC Comics - Bombshells Annual Vol.2016 01.cb7',function(err,data){
        if(err){
            console.log(err);
            return;
        }
        console.log(data.toString('base64'));
    });
});