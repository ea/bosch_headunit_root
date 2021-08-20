# Intra-process and Intra-OS Communication 

As already established, Lcn2kai platform runs in DualOS configuration with an RTOS and Linux.  There are a number of processes running on both sides and they need to communicate over the OS and process boundary. RTOS has the most direct access to the vehicle hardware and therefore all the interesting data from devices and sensors originates from RTOS side. In order to make use of it, various IntraOS and IPC mechanisms needed to be reverse engineered. This document will present the processes that are present on both sides, an overview of the OS abstraction layers , configuration store, message passing queues and other platform mechanisms. 

#### Table Of Contents

- [Running processes](#running-processes)
  * [RTOS Processes](#rtos-processes)
  * [RTOS Drivers](#rtos-drivers)
  * [Linux Processes](#linux-processes)
- [LibOSAL](#libosal)
- [Registry](#registry)
- [Message Queues](#message-queues)
  * [Application IDs and Service IDs](#application-ids-and-service-ids)
  * [Functions and opcodes](#functions-and-opcodes)
  * [Queues](#queues)
  * [Messages](#messages)
- [KDS](#kds)


## Running processes

RTOS that runs lcn2kai is based on T-kernel/T-engine and is often refered to as triton. Along the kernel itself which gets booted up via u-boot, the RTOS side consists of two sets of processes. One set might be considered "userland" while the other seems more akin to drivers. Both sets of binaries are stored in nor partition in packaged or compressed state. 

### RTOS processes

Under `nor0/processes`, the following binaries can be found:
```
-rw-r--r-- 1 400 401  843212 Aug  2  2015 procbase.out
-rw-r--r-- 1 400 401 1134260 Aug  2  2015 procdab.out
-rw-r--r-- 1 400 401  796676 Aug  2  2015 procearly.out
-rw-r--r-- 1 400 401  395276 Aug  2  2015 procipodaut.out
-rw-r--r-- 1 400 401 4147204 Aug  2  2015 procmw.out
-r--r--r-- 1 400 401 9547516 Aug  2  2015 PROCNAV.OUT
```
At first glance, these appear to be an unknown file format with various string references:

```
00000000: 584f 5a4c 0000 0000 0100 0000 0200 0000  XOZL............
00000010: 0000 0000 2400 0000 045d 1900 5ddd 0c00  ....$....]..]...
00000020: 7ef0 4814 057f 454c 4601 0101 00e0 0001  ~.H...ELF.......
00000030: 0200 2800 7401 000b 401d 1300 3400 0000  ..(.t...@...4...
00000040: ac5a 1900 0200 0005 3400 2000 0800 2800  .Z......4. ...(.
00000050: 0f00 0e00 065c 026c 0301 3480 0000 6c00  .....\.l..4...l.
```

Rudimentary reverse engineering reveals these to actually LZO compressed ELF files with a custom LZO header. You can see a script called `inflate_bins.py` in `rtos_interactions` that perfectly decompresses these.

```
d_len , c_len ,ecrc32= struct.unpack("III",cd[24:36])
print("Compressed data size: %x"%c_len)
print("Decompressed data size: %x"%d_len)

dd = lzo.decompress(cd[36:c_len+36],False,d_len)

print("Actual decompressed len: %x"%len(dd))
ccrc32 = lzo.crc32(dd)
print("Expected CRC32: %x"%ecrc32)
print("Computed CRC32: %x"%ccrc32)
print("CRC32 OK? ",ecrc32==ccrc32)

fd = open(sys.argv[1]+'.elf','wb')
```

Notably, binary `PROCNAV.OUT` is not only compressed, but actually XORed with a single byte key. It is unknown why original developers went through the trouble. 

Now that the files are decompressed, it can be confirmed that they are indeed ELFs and easily loadable by your favorite disassembly tools. Note tjat `PROCNAV.out` appears to have symbols, which is most useful! These processes have descriptory names:

- procbase.out - base process, runs first, does initial setup
- procdab.out - ???
- procearly.out - ???
- procipodaut.out - iDevice communication
- procmw.out - RTOS side of middleware implementation , wraps sensors and various services , most important
- PROCNAV.OUT - RTOS side of navigation subsystem - huge

 Services and message passing mechanisms will be detailed later, but most important ones are implemented by these processes. When trying to understand the data sent in messages, these are the first place to start looking. 

### RTOS Drivers 

"Drivers". Additional components loaded by the RTOS also come in a form of ELFs, but are somehow distinct from the above processes. These reside in `rfd_file.bin` in the firmware. Upon closer inspection, this file turns out to be a flat archive of ELFs with a minimal header describing file offset, lenght and name:

```
00000000: 0528 0000 3412 0000 1200 0000 7019 0000  .(..4.......p...
00000010: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000020: 0000 0000 0000 0000 0000 0000 0000 0000  ................
00000030: 0000 0000 0000 0000 0100 0000 e41f 0500  ................
00000040: 0000 0000 0004 0000 7379 7370 726f 676f  ........sysprogo
00000050: 7361 6c2e 756f 7574 0000 0000 0000 0000  sal.uout........
00000060: 0000 0000 0000 0000 0000 0000 0200 0000  ................
00000070: a4c1 0200 0000 0000 0024 0500 7379 7370  .........$..sysp
00000080: 726f 6763 616e 2e75 6f75 7400 0000 0000  rogcan.uout.....
00000090: 0000 0000 0000 0000 0000 0000 0000 0000  ................
000000a0: 0300 0000 0c35 0900 0000 0000 c0e5 0700  .....5..........
000000b0: 7379 7370 726f 6766 6c61 7368 2e75 6f75  sysprogflash.uou
000000c0: 7400 0000 0000 0000 0000 0000 0000 0000  t...............
000000d0: 0000 0000 0400 0000 44a6 0700 0000 0000  ........D.......

```

The unpacked file list is as follows:

 - audio_stm.uout
 - sysproganalysis.uout
 - sysprogaudio.uout
 - sysprogcan.uout
 - sysprogdnl.uout
 - sysprogdrv.uout
 - sysprogfilesys.uout
 - sysprogflash.uout
 - sysproginput.uout
 - sysprogod.uout
 - sysprogosal.uout
 - sysprogosalutil.uout
 - sysprogrvc.uout
 - sysprogsensor.uout
 - sysprogspm.uout
 - sysprogusbf.uout
 - sysprogvirtio.uout
 - xml_db.uout

I called them drivers because some obviously provide impotant mechanisms to the kernel itself. For example, `sysprogfilesys.uout` provides support for FAT file systems, `*osal` is various OS abstratcion layers which are mirrored on the Linux side, and `sysprogvirtio` provides an interface that facilitates shared virtual memory between two OSes.  I haven't yet spent too much time looking into these, but a couple are of obviously interesting, like `sysprogcan.uout` which provdes CAN bus monitoring. 


### Linux Processes

Bosch specific processes on Linux side are readily accessible under `/opt/bosch/processes` and consist of the following:

 - procbaselx_out.out - linux counterpart of the RTOS base process, starts all others
 - proccgs_out.out  - ???
 - prochmi_out.out  - MAIN head unit process, user interactions and interactions with the rest of the system go through it
 - procmapengine.out - maps something?
 - procmedia_out.out  - media management
 - procmediaplayer_out.out  - player
 - procmwlx_out.out  - linux side of middleware implementation, second most important besides HMI
 - procphone_out.out  - 
 - procsds.out - related to voice recognition?!?
 - procsds_ts.out - ?
 - procsmartphone_out.out - smartphone integration 
 - procsql_out.out  - 
 - procsxm_out.out - SiriusXM 
 - procvoice_out.out - or is this voice and not SDS?

 Obviouslly, from my notes, I didn't look into all of these. Mostly focused on the base, middleware and HMI processes. All of these binaries are regular ELFs and, most importantly, they have symbols! They all share common C++ frameworks which are much easier to navigate with symbols. These will be detailed when discussing services and message queues. 


## LibOSAL

All the processes mentioned so far use one common library, `libosal` which implements an OS abstraction layer. The API is shared on both Linux and RTOS side and is the direct way to access other IOSC and IPC mechanisms. It's fairly easy to undestand and to reuse it, the easiest way being to load it dynamically and re-export required symbols via dlopen/dlsym. Here's a list of most useful exports with guessed function prototypes:

```
int (*OSAL_IOOpen)(const char*,int param);
void (*OSAL_s32IOClose)(int fp);
int (*OSAL_s32IORead)(int fp,char *buff,int code);
int (*OSAL_s32IOControl)(int fd,int code, int *contents);
int (*OSAL_s32MessagePoolCreate)(ulong size);
int (*OSAL_s32MessagePoolOpen)();
int (*OSAL_s32MessagePoolGetCurrentSize)();
MessageStruct * (*OSAL_pu8MessageContentGet)(unsigned int *p1,unsigned int *p2,unsigned int code);
int (*OSAL_s32MessageQueueCreate)(char *queuename,int x, int y,int z, char *unknown);
int (*OSAL_s32MessageQueueOpen)(char *queuename,int x, int *unknown);
int (*OSAL_s32MessageQueueStatus)(int queueid,int *x, int *y);
int (*OSAL_s32MessageQueuePost)(int queueid, char *message, int len, int y);
int (*OSAL_s32MessageCreate)(int handle, int messageSize, int flag);
int (*OSAL_s32MessageQueueClose)(int handle);
int (*OSAL_s32MessageQueueDelete)(char *queue_name);
int (*OSAL_s32MessageQueueWait)(int queueid,int **x, int i,int **y,int **z);
```

Most important of these are file operations (open,read, write ) and IOControl especially so. Many devices exported by the RTOS are only available through special IOCTLs. Likewise, message queue operations are supported through these outlined APIs. 


## Registry

Much to my amusement, it was discovered that configuration for RTOS and Linux processes and their associated services is kept in a data store akin to Windows registry or all things! 

```
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDS/SWBLOCKS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDS/SYNCBLOCKS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDSTS/SWBLOCKS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDSTS/SYNCBLOCKS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/PROCSDSTS
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/REGISTRY/REGION/001
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/REGISTRY/REGION/002
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/REGISTRY/REGION/003
/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/CONFIG/REGISTRY/REGION/004

```

All of registry contents is accessed through `/dev/registry` and all of contents is under `/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/`. Proof of concept implementation of registry interactions is in `rtos_interactions/rtos_registry_testing.c`. It's fairly easy to dump out the whole thing. Interacting with the registry is a good demonstration of using libOSAL APIs:

```
  	int fp = OSAL_IOOpen(argv[1],4); // registry path we want to access
  	printf("listing registry key: %s\n",argv[1]);
    
    // get sub path
    ulong b1[2];
    char b2[200];
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    int b = 0 ; 
    printf("subkey:\n");
    while ( b =  OSAL_s32IOControl(fp,5,(char*)&readbuff) != -1){
    printf("%s\n",(char*)&readbuff[2]);
    }
    
    // get value name
    printf("value names: \n");
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    while( b = OSAL_s32IOControl(fp,2,(char*)&readbuff) != -1){
    printf("%s\n",(char*)&readbuff[2]);
    }
    
    //query value data
    readbuff[0] = (unsigned int)argv[2]; // registry key we want to read
    readbuff[1] = 200;
    readbuff[2] = 0;
    readbuff[3] = (unsigned int)&b2;
     b = OSAL_s32IOControl(fp,1,(char *)&readbuff);
    printf("value read retval %d %d %d %ld\n",b,readbuff[2],b2[0],b1[1]);
    printf("data: %s", b2); // some key values are actually strings
```

I would really like to know the reason behind emulating registry. 


## Message queues

To provide communication between all processes across the OS boundary, special message passing interface is employed. In short, applications that need to be notified of certain events or want to receive certain updates subscribe to services to have messages delivered to them. 
On the high level, most applications provide services that send updates via messages. It's a bit backwards to the usual message queue or subscriber/publisher setup. Here, an application that wants to receive update messages from a service would create a message queue of it's own and register with the service. Then, the service sends appropriate messages to that queue. To explan this more closely , let's go over applications and services first. 

### Application IDs and Service IDs 

All of the processes on Linux and RTOS side have a one or _MORE_ APP ID associated with them. These seem to be static and can be looked up in the registry:
```
(process  - app name - app id)
base -  spm    - 0x00000000 (0)
base - el_gateway    - 0x00000116 (278)
base - spmslv    - 0x0000001c (28)
fc_mediaplayer - fc_mediaplayer    - 0x00000059 (89)
fc_mediaplayer - ipodcontrol    - 0x00000066 (102)
procdab -  dabtuner    - 0x0000002e (46)
procearly -  rvc    - 0x00000031 (49)
procearly -  animation    - 0x00000051 (81)
procearly - vd_dimming    - 0x00000024 (36)
prochmi - hmiapp    - 0x00000109 (265)
procipodaut - vd_ipodauth    - 0x0000005a (90)
procmedia - compressedaudio    - 0x0000010e (270)
procmwlx - ipod    - 0x00000033 (51)
procmwlx - audioroutemgr    - 0x00000089 (137)
procmwlx - fc_ups    - 0x0000003b (59)
procmwlx - vd_devicemanager    - 0x0000001b (27)
procmwlx - fc_demo    - 0x00000800 (2048)
procmw - vd_kbd    - 0x00000020 (32)
procmw - vddiaglog    - 0x00000100 (256)
procmw - diag    - 0x00000114 (276)
procmw - uam    - 0x00000025 (37)
procmw - clock    - 0x00000029 (41)
procmw - vd_mmgr    - 0x00000021 (33)
procmw - meter    - 0x00000049 (73)
procmw - mcan_tcu    - 0x00000090 (144)
procmw - vd_adas    - 0x0000003d (61)
procmw - vehicle_data    - 0x0000002f (47)
procmw - audiomanager    - 0x00000107 (263)
procmw - streamrouter    - 0x00000052 (82)
procmw - audiocues    - 0x00000050 (80)
procmw - audioplayer    - 0x0000010f (271)
procmw - sensor    - 0x00002739 (10041)
procmw - sensor    - 0x00002739 (10041)
procmw - heatctrl    - 0x00000026 (38)
procmw - tmctuner    - 0x00000108 (264)
procmw - tunermaster    - 0x0000011e (286)
procmw - fc_tpeg    - 0x00000092 (146)
procmw - fc_download    - 0x0000000e (14)
procphone - phone    - 0x0000000a (10)
procsmartphone - smartphoneintegration    - 0x0000011d (285)
procsxm - fc_sxm    - 0x0000008c (140)
procvoice - voice    - 0x00000008 (8)

```

So , from the above we can see that `sensor` APP is implemented in `procmw` , or middleware, process and has an APP ID 10041. Additionally, most of the applications have services that they offer with each service having a unique service ID. This can also be gathered from registry:

```
	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/BASE/SPM/SERVICES/SPM
0	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/MAP/MAPENGINE/SERVICES/MAPENGINE
0	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/VC_SPF/SERVICES/SVC_VOICECONTROL
1	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/RECOGNIZER/SERVICES/SVC_RECOGNIZER
2	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/TTS/SERVICES/SVC_TTS
3	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/PROMPTPLAYER/SERVICES/SVC_PROMPTPLAYER
5	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/SDP/SERVICES/SVC_SDP
6	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/RESOURCE_INFORMATION
7	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSTSAPP/TRANSCRIPTIONSERVICE/SERVICES/SVC_TRANSCRIPTIONSERVICE
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/LOCATION_INPUT
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/TMCTUNER/SERVICES/TMCTUNER
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCVOICE/VOICE/SERVICES/SVC_ACR
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/PROMPTPLAYER/SERVICES/SVC_ACR
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/RECOGNIZER/SERVICES/SVC_ACR
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/TTS/SERVICES/SVC_ACR
8	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/VC_SPF/SERVICES/SVC_ACR
9	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/SBS_SUPPORT
9	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/SDSAPP/RECOGNIZER/SERVICES/SVC_VOCON
10	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/NAVI
11	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/SENSOR/SERVICES/SENSORS
13	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/TIMA/SERVICES/TRAFFIC_MESSAGE_PRESENTATION
13	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSMARTPHONE/FC_SMARTPHONEINTEGRATION/SERVICES/FC_SMARTPHONEINTEGRATION
14	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/TIMA/SERVICES/TRAFFIC_MESSAGE_DATA
16	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCVOICE/VOICE/SERVICES/VOICE
17	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/TMC_LIST_DATA
17	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMEDIA/FC_COMPRESSEDAUDIO/SERVICES/FC_COMPRESSEDAUDIO
18	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/DAPI_ROADNETWORK
20	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/RDD
21	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/MAP
22	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/NAVI_INTERNAL
23	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_DIAGNOSIS/SERVICES/FC_DIAGNOSIS
24	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/LIM
24	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_DOWNLOAD/SERVICES/DOWNLOAD
25	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/BASE/GAW/SERVICES/GAW
30	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/POS
30	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/TUNERMASTER/SERVICES/TUNERMASTER
31	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/GUIDANCE
32	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/SENSOR/SERVICES/SENSORS_IERROR
37	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCHMI/HMICCA_CLIENT/SERVICES/VR_HMI
38	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/FASTMAP
39	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCEARLY/VD_DIMMING/SERVICES/VD_DIMMING
42	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_HEATCTRL/SERVICES/FC_HEATCTRL
44	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/ROUTECALCULATION
46	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/FC_MEDIAPLAYER/FC_MEDIAPLAYER/SERVICES/MEDIAPLAYER
46	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/TMC
48	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/TEA/SERVICES/HORIZON
49	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_UAM/SERVICES/FC_UAM
50	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/WDB/SERVICES/WEATHER
52	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/WDB/SERVICES/INTWDB
54	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VD_CLOCK/SERVICES/VD_CLOCK
58	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCPHONE/FC_PHONE/SERVICES/PHONE_GENERAL
59	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCPHONE/FC_PHONE/SERVICES/PHONE_AUDIO
60	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/REGIONSELECTION
63	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VD_VEHICLE_DATA/SERVICES/VD_VEHICLE_DATA
65	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCEARLY/VD_RVC/SERVICES/VD_RVC
68	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VD_ADAS/SERVICES/VD_ADAS
76	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMWLX/fc_ups/SERVICES/FC_UPS
82	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/NAVAPP/NAVI/SERVICES/MAPCTRL
86	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/DATA_SERVER/SERVICES/POISERVER
89	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_METER/SERVICES/FC_METER
94	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMWLX/VD_DEVICEMANAGER/SERVICES/VD_DEVICEMANAGER
95	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/LBASE/SPMSLV/SERVICES/SPMSLV
96	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCEARLY/FC_ANIMATION/SERVICES/FC_ANIMATION
97	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_STREAMROUTER/SERVICES/FC_STREAMROUTER
101	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCIPODAUT/VD_IPODAUTH/SERVICES/IPODAUTH
112	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VD_DIAGLOG/SERVICES/VD_DIAGLOG
118	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/FC_MEDIAPLAYER/IPODCONTROL/SERVICES/IPODCONTROL
124	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCDAB/FC_DABTUNER/SERVICES/FC_DABTUNER
125	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/FC_LBS/SERVICES/FUELPRICES
126	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/DAPIAPP/FC_LBS/SERVICES/MOVIETIMES
128	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VDKEY/SERVICES/VDKEY
129	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/VDMMGR/SERVICES/VDMMGR
136	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMWLX/DEMO_SERVER/SERVICES/DEMO_SERVER
147	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMWLX/FC_AUDIO_ROUTE_MGR/SERVICES/FC_AUDIO_ROUTE_MGR
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMEDIA/FC_COMPRESSEDAUDIO/SERVICES/ARL
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_AUDIOPLAYER/SERVICES/FC_AUDIOPLAYER
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/TUNERMASTER/SERVICES/ARL
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCPHONE/FC_PHONE/SERVICES/AUD_SRC_ROUTE
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSMARTPHONE/FC_SMARTPHONEINTEGRATION/SERVICES/AUD_SRC_ROUTE
149	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/AUDIO
151	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/FUEL
152	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/AGW
153	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/TABWEATHER
155	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/STOCKS
156	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/MOVIES
159	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/SAFEVIEW
161	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCSXM/FC_SXM/SERVICES/CHANNELART
163	/dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT/PROCESS/PROCMW/FC_MCAN_TCU/SERVICES/FC_MCAN_TCU

```
From the above, we can see, for example, that process `PROCMW` (with application ID 10041) has a service called `SENSORS` with service ID 11. Most of the services seem to be related to navigation, or come from telematics. Like, it seems SiriusXM process would offer fule price and weather update service. With subscription required, ofc. I've found sensor, position and car diagnostics services most interesting for now. 


### Functions and opcodes

Digging into implementations of the above services and receivers of the associated messages, it is apparent that each service offers a different set of messages, or functions. Functions in turn have opcodes associated with them. There apears to be several types of functions and associated opcodes. A function can either simply request information, invoke a method to get a result in return or register for constant updates on events. 

The list of all the function IDs is very long to include verbatim, but the (incomplete) one can be found in this repository in either `services_sorted.csv` which is easily greppable, or in `services.json` which can be queried with `jq` for nicer output. Here's some examples: 

```
<service name>  <function name>   <serv id><func id><opcode>
cmb_sensorfi_ , Gyro3dData_UpdateGet ,11,65,1,,,
cmb_sensorfi_ , Gyro3dData_UpdateRelUpReg ,11,65,4,,,
cmb_sensorfi_ , Gyro3dData_UpdateStatus ,11,65,6,,,
cmb_sensorfi_ , c ,11,65,3,,,
cmb_sensorfi_ , Gyro3dDiagDataGet ,11,60,1,,,
cmb_sensorfi_ , Gyro3dDiagDataStatus ,11,60,6,,,
cmb_sensorfi_ , Gyro3dGetHwInfoMethodResult ,11,64,7,,,
cmb_sensorfi_ , Gyro3dGetHwInfoMethodStart ,11,64,2,,,
cmb_sensorfi_ , GyroADCBitRangeGet ,11,28,1,,,
cmb_sensorfi_ , GyroADCBitRangeSet ,11,28,0,,,
cmb_sensorfi_ , GyroADCBitRangeStatus ,11,28,6,,,
cmb_sensorfi_ , GyroData_InitGet ,11,3,1,,,
cmb_sensorfi_ , GyroData_InitStatus ,11,3,6,,,
cmb_sensorfi_ , GyroData_UpdateGet ,11,4,1,,,
cmb_sensorfi_ , GyroData_UpdateRelUpReg ,11,4,4,,,
cmb_sensorfi_ , GyroData_UpdateStatus ,11,4,6,,,
cmb_sensorfi_ , GyroData_UpdateUpReg ,11,4,3,,,

```

These have been extracted from the binaries that implement them. Note the first column, `cmb_sensorfi`. All of these have the form of `namespace_<service_name>fi...` so it's easy to spot to which service they belong. So, for example, last entry above shows function `GyroData_UpdateUpReg` for service `sensor` with ServiceID 11, function ID 4 and opcode 3. 

Opcodes are the same across all the services:

```
0x0 opcode_set
0x1 opcode_get
0x2 opcode_method_start
0x3 opcode_update_register
0x4 opcode_update_release
0x5 opcode_abort
0x6 opcode_status
0x7 opcode_method_result
0x8 error
0x9 opcode_abort_result
0xa opcode_method_result_first
0xb opcode_method_result_middle
0xc opcode_method_result_last
``` 

So an opcode 3 means the message is a request to register for updates, while 4 unregisters from updates for the particular function ID. Similarly, opcode 2 will invoke a function that should immediatelly return a result with opcode 7. 

I invite you to take a closer look at `services.json` and query it with `jq` to find interesting messages. For example, to see all messages sent/received by service 11 issue:

```
cat services.json | jq '.[]| select(.u16GetServiceID == 11)|  . '
```
Which should reply with something like:
```
...
{
  "u8GetOpCode": 6,
  "u16GetFunctionID": 9,
  "component": "cmb_sensorfi_",
  "name": "RimDiameterStatus",
  "u16GetServiceID": 11
}
{
  "u8GetOpCode": 1,
  "u16GetFunctionID": 56,
  "component": "cmb_sensorfi_",
  "name": "GalaSpeedGet",
  "u16GetServiceID": 11
}
{
  "u8GetOpCode": 2,
  "u16GetFunctionID": 32,
  "component": "cmb_sensorfi_",
  "name": "GyroSelftestMethodStart",
  "u16GetServiceID": 11
}

```

Or to find accelerometer related functions to subscribe to:

```
cat services.json | jq '.[]|  select(.u8GetOpCode == 3)  | select(.component == "cmb_sensorfi_")| select( .name | contains("Acc")) | .' 
{
  "u8GetOpCode": 3,
  "u16GetFunctionID": 67,
  "component": "cmb_sensorfi_",
  "name": "Acc3dData_UpdateUpReg",
  "u16GetServiceID": 11
}
```

Again, the list of services and associated function IDs and opcodes might be wrong in places and is likely incomplete as it was generated by a crude Ghidra script. 

### Queues

We've covered services and functions and mentioned messages. Where are those sent to and from? Next concept to cover is message queues. Each application can have one or more message queues created by itself. Most of these have a name in the form of "mbx_<app_id>". So a queue for SENSOR app would be `mbx_10041` for example. 

An application can create queues of it's own to receive messages into and can also open existing queues of other applications to send messages into. This is how intra process and intra OS communication happens. Application A creates queue `mbx_A` and opens queue "mbx_B" of application B. Application A then sends a message into queue "mbx_B" to request an update and then application B sends a reply into  application A's queue "mbx_A". It's fairly simple and will be more clear with an example later. 

Creating , opening and fetching messages from the queues is done via appropriate libOSAL APIs:

```
int (*OSAL_s32MessageQueueCreate)(char *queuename,int x, int y,int z, char *unknown);
int (*OSAL_s32MessageQueueOpen)(char *queuename,int x, int *unknown);
int (*OSAL_s32MessageQueueStatus)(int queueid,int *x, int *y);
int (*OSAL_s32MessageQueuePost)(int queueid, char *message, int len, int y);
int (*OSAL_s32MessageCreate)(int handle, int messageSize, int flag);
int (*OSAL_s32MessageQueueClose)(int handle);
int (*OSAL_s32MessageQueueDelete)(char *queue_name);
```

From reverse engineering, it is my understanding that only the owner application should be listening for messages in it's own queue. But, for testing purposes, it is possible to attach to any open queue and listen for all messages. This was immensly helpful while trying to undestand all the communication, but obviously leads to instabilities. On a fully booted up system, there are a couple of dozen queues created by all the processes. The partial list of avaliable queues is as follows and the complete one can be found in `nessage_queues.txt`:

```
LI_TERM_MQ
TE_TERM_MQ
OSAL_CB_HDR_LI_MAIN
NOIOSC_CB_HDR_LI_18
OSAL_CB_HDR_TE
LI_PRM_REC_MQ
PRM_MSGQUEUE
DpMaster
Dp0000008b
mbx_28
mbx_0
mbx_51
MQB_51
mbx_2048
MQB_2048
mbx_59
MQB_59
mbx_276
DVM_Worker
StateTbl
PrmMgrQ
mbx_265
MQServStateChg
GpioIf
MQTimerMngr
mbx_264
mbx_49
mbx_32
mbx_10041
mbx_36
mbx_41
mbx_38
mbx_37
```

Besides already mentioned `mbx_NNN` queues, I haven't yet explored what exactly is going on in the other ones. 


### Messages

So , what are these messages I'm talking about? As mentioned, in order to get data from another service, an application sends a specific message into the queue and then receives a reply into it's own queue. Thing get much cleared with code examples, so here's how the messages look like. First part is the message structure:

```
typedef struct messageStruct {
    short unknown1;
    short queueNum;
    int numBytes;
    short unknown2;
    char unknown3;
    char msgType;
    short unknownWord4;
    short queueSubId;
    int time;
} MessageStruct ;

``` 
Known fields are pretty self explanatory. Field `queueNum` refers to the NNN part in `mbx_NNN`, `numBytes` is the size of the message and so forth. Message type is one of the several, most common being a "normal" message of 0x45:

```
0x41 ail_bHandleMsgServiceRegister
0x42 ail_bHandleMsgServiceUnregister
0x43 ail_bHandleMsgServiceRegisterConf
0x44 ail_bHandleMsgServiceStatus
0x45 ail_bHandleMsgServiceData
0x50 ail_bHandleMsgAppInfoMessage
0xff ail_bHandleMsgDebugApplication
0x40 ail_bHandleMsgPowerMessage
```

From the above, when registering for a service, a sequence of messages should be 0x41 for registration , then a reply with 0x43 confirming it then 0x44 service status message and finally one or more 0x45 service data messages. 

Second part of the whole message is message header and actual payload:

```
typedef struct messageDataStruct {
	uint32_t magic;
	uint32_t unknown2;
    uint32_t blocks;
    uint16_t sender;
    uint16_t receiver;
    uint32_t size;
    uint16_t unknown3;
    uint8_t unknown4;
    uint8_t type;
    uint16_t s_sub;
    uint16_t d_sub;
    uint32_t time;
    uint16_t serv_id;
	uint16_t unknown5;
    uint16_t func_id;
    uint8_t opcode;

} MessageDataStruct;
```

Magic is always `0xcada` and the message chunk (after payload) ends with 0xdaca. The code uses this to check for overflows and memory corruptions. Blocks and size fields denote the size of the message as messages seem to be allocated in 12 byte blocks. Field `sender` is the sender application ID. Likewise, `receiver` is the destination APP ID. Field `type` corresponds to the message type in outer header. Field `serv_id` reffers to the service ID this message is being sent to or from, with also suitable function IDs and opcodes. Some of these fields serve dual purpose sometimes. For example, when message type is 0x41, fields "uknown5" and `func_id` serve as service major and minor version which must match what the service offers. Likewise, when service replies with `ail_bHandleMsgServiceRegisterConf`, field `unknown5` holds registration ID which must be used in all subsequent messages. 

Let's take a look at an actuall message from the service:

```
unk1:2739 queue#:0539, bytes:000000ec unk2:0002 unk3:00 msgType:45 unk4:0000 qSubId:0000 time:0000eb96
mgk:0x10daca unk2: 0xffffffff #blk:22 sndr:10041 rcvr: 1337 sz:236 unk3:0x02 unk4:0x0 typ:69 s_sub:0x00, d_sub:0x00, time:60310 serv_id:11, unk5:0x09, fn:4 op:6
cada1000ffffffff1600000039273905 ec000000020000450000000096eb0000  ............9'9........E........
0b000900040006de0000000014000000 b1e70000020800000000e3e700000308  ................................
0000000015e8000002080000000047e8 000003080000000079e8000002080000  ..............G.........y.......
0000abe80000020800000000dde80000 0408000000000fe90000010800000000  ................................
41e9000003080000000073e900000108 00000000a5e90000030800000000d7e9  A.........s.....................
000003080000000009ea000001080000 00003bea0000ff07000000006dea0000  ..................;.........m...
0208000000009fea0000050800000000 d1ea000002080000000003eb00000208  ................................
0000000035eb000006080000000067eb 00000408000000003d82ffffdaca1000  ....5.........g.........=.......
ffffffff16000000                                                   ........

```
In the above , we can see that the message originates from APP ID 10041 (we already know that's SENSORS app) and that it is being received by our application (fake id 1337). Message type is 65 (0x45) meaning it's service data. Service ID is 11 which is consistent with SENSORS service. Opcode is 6 which likewise corresponds to status message. We can query our list to see what the actual message means:

```
$ cat services.json | jq '.[]|  select(.u16GetServiceID ==11)  | select(.u16GetFunctionID == 4)| select(.u8GetOpCode == 3)| .
' 
{
  "u8GetOpCode": 3,
  "u16GetFunctionID": 4,
  "component": "cmb_sensorfi_",
  "name": "GyroData_UpdateUpReg",
  "u16GetServiceID": 11
}

```

So, the contents of the message should give us gyro sensor data. Next part is to actually figure out the payload which in this case is fairly simple:
```
00000000
14000000
b1e70000 0208 0000 0000
e3e70000 0308 0000 0000
15e80000 0208 0000 0000
47e80000 0308 0000 0000
79e80000 0208 0000 0000
abe80000 0208 0000 0000
dde80000 0408 0000 0000
0fe90000 0108 0000 0000
41e90000 0308 0000 0000
73e90000 0108 0000 0000
a5e90000 0308 0000 0000
d7e90000 0308 0000 0000
09ea0000 0108 0000 0000
3bea0000 ff07 0000 0000
6dea0000 0208 0000 0000
9fea0000 0508 0000 0000
d1ea0000 0208 0000 0000
03eb0000 0208 0000 0000
35eb0000 0608 0000 0000
67eb0000 0408 0000 0000
3d82ffff
```
The structure is fairly obvious. Second DWORD tells us how many subsequent packets of data there are (20 in this case) and then each packet consists of the same fields. First one is incremental and corresponds to the clock time when the Gyro update was measured , then second WORD is actuall data, the rest are always zero. This single word of data represents an update of Gyro's rate of change in one axis. I was really disappointed to see this. I was hoping to get all three dimensions which would make for a fun vizualization, but gyro that is built into my devices reports only yaw rate of change sadly. But i digress...

A proof of concept custom application that registers with a service for updates on certain events or data change can be found in `rtos_messages_register.c`. Here's an abridged commented version (excuse the casting attrocities):

```
    int qc_result = OSAL_s32MessageQueueCreate("mbx_1337",100,0x38,4,&qc_handle); // create our own queue
    int result = OSAL_s32MessageQueueOpen("mbx_10041",4,unk); // open target service queue


    int msg_handle = OSAL_s32MessageCreate(unk,26,2); // create registration message
    msg_handle = unk[1];
 
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)unk[0],msg_handle,4); // get the message header by message handle
    //populate struct
    message->unknown1 = 1337;
    message->queueNum = 10041;
    message->numBytes = 26;
    message->unknown2 = 2; 
    message->unknown3 =  0;
    message->msgType =  0x41;
    message->unknownWord4 =  0;
    message->queueSubId = 0;
    message->time = 0x4141;
    

    MessageDataStruct *p1 = pu32GetSharedBaseAdress();
    p1 = (MessageDataStruct*)((char*)p1+ (int)msg_handle * 0xc); // get actuall message data buffer
    memset(p1+39,0x00,30);
    //populate message data
	p1->sender = 1337;
	p1->unknown2 = 0xffffffff;
	p1->receiver = 10041;
	p1->size = 26;
	p1->type = 0x41;
	p1->unknown3 = 2;
	p1->unknown4 = 0;
	p1->s_sub = 0;
	p1->d_sub = 0;
	p1->time = 0x1231;
	p1->serv_id = 11;
	p1->unknown5 = 4; //version number major
	p1->func_id = 0xc;   //version number minor
	p1->opcode = 0;
	p1->x = 0xffff; // these don't seem to do anything
	p1->y = 0x112; // 

    result = OSAL_s32MessageQueuePost(queueid,unk,0x8,0); // post the registration request to target service

    while(OSAL_s32MessageQueueWait(qc_handle,(int**)&x,8,0,1000)<1); // wait for service to reply

    unsigned int *msg_handle = (unsigned int*)x[1]; // fetch the actuall message by message handle
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)x[0],msg_handle,4);

    //hopefully, the server replied with confirmation, now we can call an actuall function to register for updates!

    int msg_handle = OSAL_s32MessageCreate(unk,32,2);
    msg_handle = unk[1];
 
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)unk[0],msg_handle,4);
        message->unknown1 = 1337;
        message->queueNum = 10041;
        message->numBytes = 32;
        message->unknown2 = 2; 
        message->unknown3 =  0;
        message->msgType =  0x45;
        message->unknownWord4 =  0;
        message->queueSubId = 0;
        message->time = p->time+5; // can't bother keeping track of time as it's always off on my test unit
        MessageDataStruct *p1 = pu32GetSharedBaseAdress();
     p1 = (MessageDataStruct*)((char*)p1+ (int)msg_handle * 0xc);
  
     p1->sender = 1337;
     p1->receiver = 10041;
     p1->size = 32;
     p1->type = 0x45;
     p1->s_sub = 0;
     p1->d_sub = 0;
     p1->time = p->time+5;
     p1->serv_id = 11;
     p1->func_id =  4; // Gyro Data Update Register
     p1->unknown5 = 0x9; //registration id
     p1->opcode =  3; //3 == register, 4 unregister ? 
     result = OSAL_s32MessageQueuePost(queueid,unk,0x8,0);

     // now we are registered and should enter a loop to wait for updates

     while(OSAL_s32MessageQueueWait(qc_handle,(int**)&x,8,0,1000)<1); // wait for service to reply

```

Some of the tricky bits here involve figuring out minor and major version of the service. I haven't found a good way to extract that info for all of the services but have found a couple through either reverse engineering or brute force. 

Hope that explains it! The message interaction examples in this repo should be illustrative, but are work in progress or simply proofs of concept. Let me know if something is unclear!


## KDS


OSAL library provides an interface to talk to certain devices exposed from RTOS. One of them is `/dev/kds`  which serves as some sort of a configuration or state data store. It seems to be backed by a file residing in flash and we can interact with it through libosal.

Examples `https://github.com/ea/bosch_headunit_root/blob/main/scripts/rtos_interactions/kds_dev_read.c` and `https://github.com/ea/bosch_headunit_root/blob/main/scripts/rtos_interactions/kds_dev_write.c` show how to read and write to certain data structures. What you will need for this to succeed is the access code or file location for each of the structures. So , for example, to read stored bluetooth device name , you could issue: 

```
./kds_dev_read 0xDFE 0x18 /dev/kds 
```

And to change this to custom value is as easy as:

```
./kds_dev_write /dev/kds 0xDFE BTNAME
```

Reading and writing is relatively simple, though when writting, writes need to be "flushed" otherwise they don't stick:

```
	OSAL_s32IOControl(fp,2,(int*)1);
	buffer.ioread.code = (unsigned short)strtol(argv[2],0,16);;
	buffer.ioread.len = strlen(argv[3]);
	strcpy(buffer.ioread.data,argv[3]);
	OSAL_s32IOWrite(fp,buffer.rawdata,0xf6); 
	OSAL_s32IOControl(fp,12,(int*)1); // flush write
```

All KDS associated addresses and structures of returned data:

- 0xD59 VehicleInformation
- 0xD61 CameraSystem
- 0xDFE BTName
- 0xD53 SystemConfig2FormerAudio
- 0xD60 ECORoute
- 0xD30-0xD38 SDSECNR (what could this acronym mean?)
- 0xD52 SystemConfiguration
- 0xD40-0xD48 PhoneECNR
- 0xD5D FMAMTunerParameter
- 0xD5F AudioParamterSet
- 0xD5B , 0xD5C - DABParameterSet
- 0xD5E PartsNumber 

Neighbour @raburton has helpfully started to document the structures in the associated issue https://github.com/ea/bosch_headunit_root/issues/13#issuecomment-838215185 
Copying it here for posterity:

**Note:** these structs are for documentation purposes - the order of bit field storage is not defined in the C standard and is implementation specific. The structs are written as you would read a dump of the data from kds, i.e. the first byte in the structure is the first byte in the data, the first bit in a bit field (as documented below) is the left most bit in the byte containing it.
Your compiler may store bit fields in reverse order, i.e. first bit field below is actually the right most bit in the byte. If this is the case you will need to reverse the order of the any bit fields, within each byte, to use these structs. Also note that your structs will need to be packed with #pragma pack(1).


```
// code: 0x0d60, len: 0x04
struct ECORoute {
	uint8  FuelType : 1;          // byte 0
	uint8  DragCoefficient : 7;
	uint8  TransmissionType : 2;  // byte 1
	uint8  unused : 6;
	uint16 crc;                   // byte 2-3
};
```
Example 20c078ea\
FuelType = 0 (from an unleaded car)\
DragCoefficient = 20\
TransmissionType = 3 (from a manual 6 speed car)\
crc = 0xea78

---

```
// code: 0x0d70, len: 0x18
struct BTName {
	uint8  Name[22];
	uint16 crc;
};
```
Example 4d59434152000000 0000000000000000 0000000000004651\
Name = "MYCAR" (null padded, max length 21 if must be null terminated or 22 if not)\
crc = 0x5146

---

```
// code: 0x0d5e, len: 0x1b
struct PartsNumber {
	uint8  PartNumber[5];
	uint8  ConfigurationHash[20];
	uint16 crc;
};
```
Example 335a4c3742...... ................ ................ ..b0e9\
PartNumber = "3ZL7B" (appears fixed length, not null terminated)\
ConfigurationHash = (hidden, in case unique)\
crc = 0xe9b0

---

```
// code: 0x0d61, len: 0x04
struct CameraSystem {
	uint8  CameraSystem : 3;        // byte 0
	uint8  AnticipatoryLine : 1;
	uint8  unused : 4;
	uint8  Guideline;               // byte 1
	uint16 crc;                     // byte 2-3
};
```
Example 80008b83\
CameraSystem = 4 (from car with birds eye view, 4 camera system)\
AnticipatoryLine = 0\
Guideline = 0\
crc = 0x838b

---

```
// code: 0x0d59, len: 0x0a
struct VehicleInformation {
	uint8  unknown1;                        // byte 0
	uint16 VehicleType;                     // byte 1-2
	uint8  CANGeneration1 : 1;              // byte 3
	uint8  Region : 4;
	uint8  SteeringWheel : 3;
	uint8  SteeringPosition : 1;            // byte 4
	uint8  unknown2 : 1;
	uint8  ShowClock : 1;
	uint8  MeterClockSynchronisation : 1;
	uint8  PlatformType : 1;
	uint8  DistanceUnitsSupported : 2;
	uint8  unknown3 : 1;
	uint8  OpeningAnimation : 2;            // byte 5
	uint8  unknown4 : 6;
	uint8  OffRoadInformation : 1;          // byte 6
	uint8  unknown5 : 1;
	uint8  VehicleConfiguration : 1;
	uint8  HEV1 : 1;
	uint8  AntiTheft : 1;
	uint8  DrivingType : 2;
	uint8  JudgementForOP : 1;
	uint8  VoiceRecognition : 1;            // byte 7
	uint8  unknown6 : 1;
	uint8  CAN : 1;
	uint8  AUXKind1 : 2;
	uint8  ISA : 1;
	uint8  unknown7 : 2;
	uint16 crc;                             // byte 8-9
};
enum Regions {USA = 1, CAN, EUR, PRC, GCC, RUS, ASR, ARG, BRA, SAF, MEX, THI, TKY};
```
Example 00124c19bc400928 8c5d\
...\
Region = 3 (from UK car)\
...\
crc = 0x5d8c

---

```
// code: 0x0106, len: 0x07
uint8 DeviceSerialNumber[7];
```

---

```
// code: 0x1216, len: 0x10
uint8 CryptedFileAesKey[16];
```
Used for .ntq alert files.

---

```
// code: 0x0104, len: 0x08
uint8 TelematicsPassword[8];
```
Not seen yet for real. Password appears to be encrypted/hashed.

---

```
// code: 0x0d53, len: 0x03
struct SystemConfiguration2formerAudio {
	uint8  unknown : 4;
	uint8  AudioOutput : 4;
	uint16 crc;
}
```

Example 01f1e1\
AudioOutput = 1\
crc = 0xe1f1

---

```
// code: 0x0d52, len: 0x41
struct SystemConfiguration1 {
	uint8  Tag[8];                         // byte 0-7
	uint8  MajorVersion;                   // byte 8
	uint8  MinorVersion[2];                // byte 9-10
	uint8  Language;                       // byte 11
	uint8  KindOfDestinationEntry : 4;     // byte 12
	uint8  unknown2 : 4;
	uint8  UserKindOfPOIWarning : 1;       // byte 13
	uint8  MapZoom : 1;
	uint8  unknown3 : 4;
	uint8  SpeedLock : 1;
	uint8  Disclaimer : 1;
	uint8  unknown4 : 3;                   // byte 14
	uint8  HMIEV : 1;
	uint8  FrequencyPSDisplay : 1;
	uint8  ShowFMStationList : 1;
	uint8  ECOFeedback : 1;
	uint8  SoundOnEncoderPress : 1;
	uint8  unknown5;                       // byte 15
	uint8  unknown6 : 6;                   // byte 16
	uint8  VoiceRecognitionHandling : 1;
	uint8  GUIHandling : 1;
	uint8  unknown7;                       // byte 17
	uint8  ECOScoreParameter1[4];          // byte 18-21
	uint8  ECOScoreParameter2[4];          // byte 22-25
	uint8  ECOScoreParameter3[4];          // byte 26-29
	uint8  ECOScoreParameter4[4];          // byte 30-33
	uint8  unknown8[4];                    // byte 34-37
	uint8  Brightness;                     // byte 38
	uint8  unknown9 : 4;                   // byte 39
	uint8  SoundParameterSetSelection : 4;
	uint8  unknown10 : 6;                  // byte 40
	uint8  SmartphoneIntegration : 1;
	uint8  Pandora : 1;
	uint8  unknown11 : 7;                  // byte 41
	uint8  AutomaticPhoneBookTransfer : 1;
	uint8  unknown12 : 5;                  // byte 42
	uint8  MinimizeUnpavedRoads : 1;
	uint8  GetPositioningSystem : 2;       // *** see note ***
	uint8  unknown13 : 7;                  // byte 43
	uint8  Dab : 1;
	uint8  unknown14[19];                  // byte 44-62
	uint16 crc;                            // byte 63-64
}
```

Note: According to the getter method GetPositioningSystem is two bits, one of which overlaps MinimizeUnpavedRoads. Presumably this is a bug, and so I have moved it down one bit.

Example
```
2020202020202020 3220200000800600
0000000000c80000 00c8000000d20000
00c8000000003200 0200000100000000
0000000000000000 00000000000000e7
6a
```

---

```
// code: 0x0d5b & 0x0d5c, len: 0xfd (see notes)
struct DABParameterSet {
	uint8  Tag[8];
	uint8  MajorVersion;
	uint8  MinorVersion[2];
	uint8  unknown[240];
	uint16 crc;
}
```
Data structure spans two kds fields. Code actually only reads 0xe6 bytes from 0x0d5b. I assume the last two bytes are the crc for the block and are skipped.
It then appends 0x17 bytes from 0x0d5c, dropping multiple bytes from the end, including what is probably the crc for the second block in the last two bytes.
Oddly the code has a getter method for the crc that points 0xfb bytes into the reconstructed structure. This is the last two bytes of the truncated structure,
but doesn't appear to point to an actual crc in the example below.


Example:
```
Code: 0d5b, len: 00e8
4441425f45555231 3230001300030000  DAB_EUR120......
0001100313070700 0000000000000000  ................
0008040000000000 00000000037ed001  .............~..
7f7f7f0000000001 0000fe4e6f205365  ...........No.Se
7276696365000000 000000010000ff4e  rvice..........N
6f20456e73656d62 6c65000000000000  o.Ensemble......
0000000000000000 0000000c0c0c0c0c  ................
0c0c0c0c0c0c0c0c 0c0d0e1012141618  ................
1a1c1e1f1f1f1f1f 1f1f1f1212121213  ................
1313141414151515 1516161717171818  ................
1919191a1a1a1a1a 1a1a1a0710170a01  ................
0b2d141405011200 000000011002033c  .-.............<
0305040505040481 810201fffe000000  ................
0007020002040207 0000640064004605  ..........d.d.F.
000000000000f902                   ........

Code: 0d5c, len: 002f
0000000000000000 0000000000000000  ................
0000000000000000 0000000000000000  ................
0000000000000000 00000000005a19    .............Z.
```

Tag = "DAB_EUR1"\
MajorVersion = "2"\
MinorVersion = "0" (null padded)\
crc = 0x0000

---

```
// code: 0x0d30 - 0x0d38, len: 0x080d (see notes)
struct SDSECNR {
	uint8  Tag[8];
	uint8  MajorVersion;
	uint8  MinorVersion[2];
	uint8  unknown[2048];
	uint16 crc;
}
```



## Trace mechanism


debugging mechanism and hooking


