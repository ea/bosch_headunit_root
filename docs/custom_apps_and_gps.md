# Example of a custom extension - GPS logger

In order to make something usefull with the access we got we mostly need two things. A way to execute custom code and a way to hook into user interface somehow. Turns out that Airbiquity middleware is exactly what we need. It runs on NodeJS, so it's easy to program and it ties into UI via `Apps` menu, giving us easy way to interact with out code. 

In this writeup, I'll show an example extension to the system that enables GPS logging. To do so, we'll simply replace Airbiquity middleware with out own. 

## Replacing Airbiquity Middleware 

As mentioned in the other writeup, HUP middleware is executed via `/bin/node` which is a shell script that simply sets up and starts Airbiquity's middleware like so:
```
/opt/bosch/airbiquity/node /opt/bosch/airbiquity/hup/hup.js
```

We can simply override this script to execute our own NodeJS code. To make it simple to restore original functionality, we'll place our custom code on a flash drive, then check for it's presence at startup. If the flash drive is present, we'll execute code from there. If not, we'll start default Airbiquity middleware. 
Something like this does the trick:

```
if [ -f "/dev/media/storage/xterra/my_server.js"  ]
then
       mount -o remount,rw /dev/media/2f72fd33-c291-4ab0-b3fa-e5eec6991323/

       /opt/bosch/airbiquity/node /bin/my_server.js
else

        /opt/bosch/airbiquity/node /opt/bosch/airbiquity/hup/hup.js | trace_tool -c 45578 -l 8
```

## Custom Server and IPC 

We know that HUP starts listening on port 8124 which it uses as a communication mechanism with the rest of the head unit. We'll do the same, but instead of acting on received data, we'll simply log it to file. 

```
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
```

Above code starts an HTTP server so that lcn2kai's `Apps` menu has something to connect to, and then starts a simple listener on port 8124. Handler for port 8124 listens for incomming data which is in a form of JSON messages and then writes them to a file.

## Sending Requests

With a little bit of reversing of `procsmartphone_out`, it's easy to figure out a list of notification requests and replies that it supports and which can be sent and received via port 8124. All of these are JSON messages which specify certain methods and parameters. 

For example, one of the useful request messages is `huGetNavInfo` which looks like this:

```
{"FID":"0x1900","MethodName":"huGetNavInfo","LocationType":"0","SeqID":"0x8002"}
```

FID is some sort of function ID and is unique per method name, method name is self explanatory and is followed by a number of parameters. Message `huGetNavInfo` has only one, `LocationType`. Location type 0 means current location, while type 1 signifies destination, if set in navigation app. 

When this message is sent to lcn2kai from HUP via port 8124, it sends a reply:

```
{"FID":"0x1901",
 "MethodName":"aqNavInfoStatus",
 "LocationType":"0",
 "s32Longitude":"XXXXXXXXX",
 "s32Latitude":"XXXXXXXXX",
 "Heading":"3",
 "SeqID":"0x0000"
 }
```

Great! The reply contains an encoded position as well as heading. lcn2kai actually sends a wealth of notification about car's state (including speed and time for example) without prompt , so it's useful to log those as well.
All we need to do now is set up our node app to periodically send `huGetNavInfo` requests and log the replies:

```
const navUpdateRequest = function(){
        if(headunit_socket != undefined)
        headunit_socket.write('{"FID":"0x1900",
                                "MethodName":"huGetNavInfo",
                                "LocationType":"0",
                                "SeqID":"0x8002"}');

}
var msgInterval = setInterval(navUpdateRequest,60*1000);
```

## Decoding coordinates 

In navigation info reply, lcn2kai replies with lattitude and longitude in encoded form for some reason. It's easy enough to figure out the encoding formula and reverse it:

```
def convert_s32latlong(n):
    f = (int(n) - 0.5 ) / 11930464.711111
    return f
```

I'd really like to know the reason for this form of encoding. 


## Putting it all together

Node script `gps_hup.js` in this repository basically does just what we described. It mimics Airbiquity and starts an lazy HTTP server and a notification request/reply channel. It logs recieved messages to the files with name of the form `gpshup-<timestamp>.json` which can later be processed and viewed. 

As mentioned at the beggining, I wanted all the custom code to be on a USB flash drive (which is also where the logs will be stored). In order to set it up, you'll need a spare USB flash drive formated with ext2. Then you need to modify lcn2kai scripts to actually run `gpshup.js`. To do so, you should do the following:

1. log into lcn2kai via ssh
2. remount root file system as RW 
```
$ mount -o remount,rw / 
```
3. Figure out where the USB flash drive with `gpshup.js` will be mounted at
  - it will be mounted at `/dev/media/<UUID>/`, to figure out UUID you can use `blkid` on your laptop
4. Modify  `/bin/node` to check for flash drive presence on startup to something like

```
if [ -f "/dev/media/ INSERT_YOUR_UUID_HERE /gpshup.js"  ]
then
       mount -o remount,rw /dev/media/ INSERT_YOUR_UUID_HERE /
        /opt/bosch/airbiquity/node /dev/media/ INSERT_YOUR_UUID_HERE /gpshup.js
else

/opt/bosch/airbiquity/node /opt/bosch/airbiquity/hup/hup.js | trace_tool -c 45578 -l 8

fi 
```
5. Save and shut down lcn2kai. 
6. Copy `gpshup.js` to your USB flash drive. 
7. Plug in USB flash drive into the car.
8. Turn on the head unit. 

If there were no errors in this process, lcn2kai should detect the flash drive on startup, and `/bin/node` script will execute `gpshup.js` instead of original one. If you go to `Apps` menu on the head unit, you should see the default app list deplaced with blank screen with HUP messages in blue (not my coordinates, probably a salvage yard that sold this unit on ebay):

![gpshup](images/04_gpshup_app.bmp)


Now it's time to drive around and  log some GPS data. 

## Viewing GPS data

Now that you've collected some GPS logs on the flash drive , it would be nice to convert them into some more usable file format. As they are, the log files will be a JSON file with an array of different HUP messages, most of them navigation data updates. Script `s32latlong_convert.py` converts these into GPX file format. GPX is a standard GPS data exchange format and most vizualization and editing tools support it. For example, you can use `gpsprune` to load, vizualize on a map and edit the recorded files. You can use `GPX-Animator` to create videos of your recorded drive. 
