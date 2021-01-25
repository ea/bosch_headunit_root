const http = require('http');
const net = require('net');
const fs = require('fs');

var last_msg = "";
var messages = [];
var headunit_socket;
var ts = Date.now();
var DEV = false;
var log_path;

//on test device , store logs to /tmp instead of flash drive
if (DEV) {
    log_path = '/tmp/';
} else {
// on device in the car, store the logs on the flash drive, into the same dir where the replacement server is
    log_path = __dirname 
}

var log_filename = log_path + "gpshup-" + ts + '.json';

//just give it something to display in Apps menu... 
var html_message = '<html>\n\n<head> <style>.button {background-color: #008CBA;}  </style> \n    <TITLE>myhup</TITLE>  \n</head>  \n\n<body> <script>setInterval(function(){ location.reload();},30*1000); </script><button type="button">Start logging</button>  <input>test</input>\n\n  <pre>  <p style="color:blue;">message  </p> </pre>\n</body>    \n</html>'


//serve the above static HTML with message replaced 
const requestListener = function(req, res) {
    res.writeHead(200, {
        'Content-Type': 'text/html'
    });
    res.end(html_message.replace('message', JSON.stringify(last_msg, null, 4)));
}


const notificationDataHanler = function(c) {

    console.log('client connected');
    headunit_socket = c;
    c.on('end', function() {
        console.log('client disconnected');
        headunit_socket = undefined;
    });
    c.on('data', function(data) {
        console.log(data.toString());
        try {
            msgs = data.toString().split('}{');
            for (var i = 0; i < msgs.length; i++) {
                last_msg = JSON.parse(msgs[i]);
                messages.push(last_msg);
            }
            fs.writeFile(log_filename, JSON.stringify(messages), function(err) {});
        } catch (e) {

            console.log(e);
        }
    });

}

const server = http.createServer(requestListener).listen(8090);
const notificationServer = net.createServer(notificationDataHanler).listen(8124);

const navUpdateRequest = function() {
    if (headunit_socket != undefined)
        headunit_socket.write('{"FID":"0x1900","MethodName":"huGetNavInfo","LocationType":"0","SeqID":"0x8002"}');

}
var msgInterval = setInterval(navUpdateRequest, 60 * 1000);

