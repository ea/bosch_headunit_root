This directory contains a couple of simple programs that demonstrate how to use some of the OSAL APIs.


To build:
```
export SYSROOT=/path/to/extracted/rootfs
make
```
Makefile has a "deploy" targer that copies binaries to `/tmp` if headunit is connected to the network.

# rtos_message_testing 
Demonstrates how to get ahold of a message queue and listen for messages. Still need to figure out actuall message contents and what different channels are for.

```
root@(none):/tmp# ./rtos_message_testing mbx_0
OSAL_s32MessagePoolCreate -1
pool current size: 2013852 
Queue id 0 1074429980 0
x 8 2 28499 0
content x 8 2 28499 0
unk1:001c queue#:0000, bytes:00000024 unk2:0002 unk3:00 msgType:45 unk4:0000 qSubId:0000 time:00206a02
00000000000000000001000000000000                                   ................


x 8 2 28671 0
content x 8 2 28671 0
unk1:001c queue#:0000, bytes:00000024 unk2:0002 unk3:00 msgType:45 unk4:0000 qSubId:0000 time:002071d0
00000000908020000508000000000000                                   ................


x 8 2 2973a 0
content x 8 2 2973a 0
unk1:001c queue#:0000, bytes:00000024 unk2:0002 unk3:00 msgType:45 unk4:0000 qSubId:0000 time:002079a0
00010000000000000000000000000000
```

# rtos_registry_testing

Demonstrates how to read registry device and get data out of it. Some data values are strings, some are ints, this prints out strings:

```
root@(none):/tmp# ./rtos_registry_testing /dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/ A
listing registry key: /dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/
subkey:
LBASE
BASE
PROCSQL
TENGINE
LINUX
CONFIG
PROCEARLY
PROCMW
PROCDAB
PROCIPODAUT
PROCNAVI
PROCMWLX
PROCSXM
PROCPHONE
```

# rtos_device_testing

Demonstrates how to open a device and read specific values. Only device that is interesting to read (that we've found so far) is `/dev/kds`. Following example is reading configured bluetooth name:

```
root@bosch-nemid:/tmp# ./rtos_device_testing 0xdfe 0x0 /dev/kds   

0dfe 0018 0000:
4d592056455253410000000000000000 0000000000008588                  MY.VERSA................

```


# trace_hooking_demo

Trace system seems to be a very extensive debugging framework of some sort, but I haven't yet spent much time to look into it. It looks like it can be very verbose, but it isn't obvious how to get the data out.
    For testing purposes, I've added a simple LD_PRELOAD hooking demo that redirects tracing messages to stdout instead of /dev/trace.
```
root@bosch-nemid:/tmp# LD_PRELOAD=./inject.so ./trace_hooking_demo 2
OsalData in SHM: 0x40000000
Tracing? 1
Tracing? 1
TRACE: Blocklist: Absolute size: 2048016, Current size: 2028756

TRACE: Free block: Real 0x2ad74030, Offset    2 (0x2), Size 1996752, Blocksize 166396 

TRACE: Deleted marked block: Real 0x2af5b800, Offset 166398 (0x289fe), Size without u16MsgOffset 4294901821, Blocksize 5 

TRACE: Free block: Real 0x2af5b83c, Offset 166403 (0x28a03), Size 264, Blocksize 22 

TRACE: Deleted marked block: Real 0x2af5b944, Offset 166425 (0x28a19), Size without u16MsgOffset 4294902025, Blocksize 22 

TRACE: Deleted marked block: Real 0x2af5ba4c, Offset 166447 (0x28a2f), Size without u16MsgOffset 4294901821, Blocksize 5 

TRACE: Deleted marked block: Real 0x2af5ba88, Offset 166452 (0x28a34), Size without u16MsgOffset 4294901821, Blocksize 5 

TRACE: Deleted marked block: Real 0x2af5bac4, Offset 166457 (0x28a39), Size without u16MsgOffset 4294901821, Blocksize 5 

TRACE: Deleted marked block: Real 0x2af5bb00, Offset 166462 (0x28a3e), Size without u16MsgOffset 4294901821, Blocksize 5 
...
```
    
