const ArchiveManager = require('../build/Release/manager');
const fs = require('fs');

var testObj = {
    title:"DC Bombshells #1"
}

var buffer = Buffer.from(JSON.stringify(testObj));

ArchiveManager.Remove(['info.json'],
    'o:/Comics/DC Comics/DC Comics - Bombshells Annual (2016 Annual)/DC Comics - Bombshells Annual Vol.2016 01.cb7',function(err,outcome){
    if(err){
        console.log(err);
        return;
    }
    var message = (outcome)?"Success":"Failure";
    console.log(message);
});