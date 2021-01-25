# System overview

In this document, I'll gather all the information about lcn2kai system that could be useful in further exploration. 

- [U-boot log](#u-boot-log)
- [DualOS](#dualos)
- [System and processes](#system-and-processes)

# U-Boot log

While lcn2kai boots, we get a lot of information from U-Boot:


```
[    0.009674]
[    0.009698] U-Boot 2010.03-00391-gf3b3496 (May 15 2014 - 16:53:57) for NEC NEmid
[    0.009754]
[    0.009772] (C) 2009-2010 Robert Bosch Car Multimedia, CM-AI/PJ-CF32, Dirk Behme
[    0.009830] CPU:      MPCore at 400MHz
[    0.009866] U-Boot    #1 (env @ 0x40080000)
[    0.009904] Board:    NEmid based LCN2kai TSB4 Sample (1G) board
[    0.009954] Board ID: 0x3007 (#1)
[    0.011105] DRAM:   1 GB
[    0.011185] PRAM cleared
[    0.011208] Reset Counter cleared
[    0.011238] Reset Counter has the value 0
[    0.011962] Flash: S-Die
[    0.012041] Flash: 64 MB
[    0.012073] *** Warning - bad CRC, using default environment
[    0.012117]
[    0.012538] In:    serial
[    0.012563] Out:   serial
[    0.012587] Err:   serial
[    0.014028] Reset Counter cleared
[    0.014063] Reset Counter cleared
[    0.014509] Net:   No ethernet found.
[    0.014592] Hit any key to stop autoboot:  0
[    0.017803] ## Booting kernel from Legacy Image at 40920000 ...
[    0.017889]    Image Name:   triton_min_dualos
[    0.017926]    Image Type:   ARM RbcmRTOS Kernel Image (uncompressed)
[    0.017987]    Data Size:    1294164 Bytes =  1.2 MB
[    0.018040]    Load Address: 80000000
[    0.018074]    Entry Point:  80000290
[    0.018140]    Loading Kernel Image ... OK
[    0.050506] OK
[    0.050965]
[    0.050978] Starting guest OS ...
[    0.051045] ## Booting kernel from Legacy Image at 40220000 ...
[    0.051146]    Image Name:   Linux-2.6.34.13-02018-g843e5c6
[    0.051192]    Image Type:   ARM Linux Kernel Image (uncompressed)
[    0.051253]    Data Size:    2076344 Bytes =  2 MB
[    0.051306]    Load Address: 86000000
[    0.051341]    Entry Point:  86000000
[    0.051389]    Loading Kernel Image ... OK
[    0.116345] OK
[    0.236196]
[    0.236213] Starting kernel ...
[    0.236239]
Uncompressing Linux... done, booting the kernel.
```

Also of use in debugging and testing stuff is U-boot environment and available commands:

```
          981] base    - print or set address offset
[   22.754025] bdinfo  - print Board Info structure
[   22.754068] bootm   - boot application image from memory
[   22.754117] bootp   - boot image via network using BOOTP/TFTP protocol
[   22.754173] cmp     - memory compare
[   22.754209] coninfo - print console devices and information
[   22.754258] cp      - memory copy
[   22.754291] crc32   - checksum calculation
[   22.754330] echo    - echo args to console
[   22.754370] em_trace- display error memory entries in PRAM
[   22.754419] erase   - erase FLASH memory
[   22.754456] exit    - exit script
[   22.754490] false   - do nothing, unsuccessfully
[   22.754533] fatinfo - print information about filesystem
[   22.754581] fatload - load binary file from a dos filesystem
[   22.754632] fatls   - list files in a directory (default /)
[   22.754682] flinfo  - print FLASH memory information
[   22.754728] go      - start application at address 'addr'
[   22.754777] help    - print command description/usage
[   22.754823] imxtract- extract a part of a multi-image
[   22.754870] loop    - infinite loop on address range
[   22.754915] md      - memory display
[   22.754950] mii     - MII utility commands
[   22.754989] mm      - memory modify (auto-incrementing address)
[   22.755041] mtest   - simple RAM read/write test
[   22.755084] mw      - memory write (fill)
[   22.755123] nm      - memory modify (constant address)
[   22.755170] pci     - list and access PCI Configuration Space
[   22.755221] printenv- print environment variables
[   22.755265] protect - enable or disable FLASH write protection
[   22.755318] rarpboot- boot image via network using RARP/TFTP protocol
[   22.755374] reset   - Perform RESET of the CPU
[   22.755417] run     - run commands in an environment variable
[   22.755468] saveenv - save environment variables to persistent storage
[   22.755525] setenv  - set environment variables
[   22.755571] setrtosaddr- set the RAM entry address for the RTOS (autoselected based on the board ID)
[   22.755646] showvar - print local hushshell variables
[   22.755693] sleep   - delay execution for some time
[   22.755737] source  - run script from memory
[   22.755778] startguestos- start guest OS on CPU #2
[   22.755823] startsingleRTOS- start RTOS as single os from flash
[   22.755875] test    - minimal test like /bin/sh
[   22.755918] tftpboot- boot image via network using TFTP protocol
[   22.755971] true    - do nothing, successfully
[   22.756013] update  - perform a recovery update
[   22.756055] usb     - USB sub-system
[   22.756090] usbboot - boot from USB device
[   22.756129] version - print monitor version
[   22.756576] NEMID # printenv
[   27.528646] bootargs=reset
[   27.528675] bootcmd=run setbootargs; if bootm start 0x40920000; then setrtosaddr; bootm loados; startguestos; fi; bootm
[   27.528769] bootdelay=0
[   27.528795] baudrate=115200
[   27.528823] loadaddr=0x40220000
[   27.528854] usbstortimeout=5
[   27.528883] verify=no
[   27.528906] cores=2
[   27.528929] ipaddr=172.17.0.1
[   27.528959] serverip=172.17.0.6
[   27.528990] rootdev2=mmcblk0p2 ro
[   27.529023] rootdev=mmcblk0p1 ro
[   27.529054] uboot=u-boot.bin
[   27.529083] linux=uImage
[   27.529109] dualos=triton_dualos.bin.uimage
[   27.529149] dualosmin=triton_min_dualos.bin.uimage
[   27.529193] rfd=rfd_file.bin
[   27.529222] ramdisk=uInitramfs
[   27.529253] xtargs=quiet ohci-hcd.distrust_firmware=0
[   27.529299] norfsfld=norfs_filled.bin
[   27.529335] norfsmpt=norfs_empty.bin
[   27.529369] panic=1
[   27.529392] panic_on_oops=1
[   27.529420] nfsroot=/opt/bosch/y/di_projects/generated/target_rootfs
[   27.529477] nfsboot=setenv rootdev nfs rw nfsroot=${serverip}:${nfsroot} ip=${ipaddr};saveenv;reset
[   27.529557] bootchart=setenv xtargs ${xtargs} initcall_debug printk.time=y init=/sbin/bootchartd;saveenv; reset
[   27.529644] startusb=usb start; setenv startusb echo 'USB started'
[   27.529700] setfatload=setenv loader fatload usb 0:1 0x80000000 ${loadfile}
[   27.529762] setftpload=setenv loader tftp 0x80000000 ${loadfile}
[   27.529817] setnorloader=echo 'no loader defined'
[   27.529861] flshb=if run loader; then protect off 0x40020000 +${filesize};erase 0x40020000 +${filesize};cp.b 0x80000000 0x40020000 ${filesize};protect off 0x40160000 +${filesize};erase 0x40160000 +${filesize};cp.b 0x80000000 0x40160000 ${filesize};fi
[   27.530047] flshkrnl=if run loader; then erase 0x40220000 +${filesize};cp.b 0x80000000 0x40220000 ${filesize};fi
[   27.530135] flshdls=if run loader; then erase 0x40920000 +${filesize};cp.b 0x80000000 0x40920000 ${filesize};fi
[   27.530223] flshdls2=if run loader; then erase 0x41020000 +${filesize};cp.b 0x80000000 0x41020000 ${filesize};fi
[   27.530312] flshdlsmin=if run loader; then erase 0x40920000 +${filesize};cp.b 0x80000000 0x40920000 ${filesize};fi
[   27.530401] flshrfd=if run loader; then erase 0x40B20000 +${filesize};cp.b 0x80000000 0x40B20000 ${filesize};fi
[   27.530489] flshffs=if run loader; then erase 0x41940000 +${filesize};cp.b 0x80000000 0x41940000 ${filesize};fi
[   27.530577] handle_norfs=erase 0x41940000 0x41afffff
[   27.530623] norfs_default=setenv handle_norfs erase 0x41940000 0x41afffff
[   27.530684] norfs_untouch=setenv handle_norfs echo norfs-untouched
[   27.530740] norfs_empty=setenv handle_norfs run ffsmpt
[   27.530787] norfs_filled=setenv handle_norfs run ffsfld
[   27.530835] ffsmpt=echo 'ffsmpt';setenv loadfile ${norfsmpt};run setnorloader; run flshffs
[   27.530908] ffsfld=echo 'ffsfld';setenv loadfile ${norfsfld};run setnorloader;run flshffs
[   27.530980] bootup=echo 'bootup';run startusb;setenv loadfile ${uboot};run setfatload;run flshb
[   27.531057] kernup=echo 'kernup';run startusb;setenv loadfile ${linux};run setfatload;run flshkrnl
[   27.531135] dualosup=echo 'dualosup';run startusb;setenv loadfile ${dualos};run setfatload;run flshdls
[   27.531216] dualosup2=echo 'dualosup2';run startusb;setenv loadfile ${dualos};run setfatload;run flshdls2
[   27.531300] dualosminup=echo 'dualosminup';run startusb;setenv loadfile ${dualosmin};run setfatload;run flshdls
[   27.531388] rfdup=echo 'rfdup';run startusb;setenv loadfile ${rfd};run setfatload;run flshrfd
[   27.531463] tftpu=run startusb;setenv loadfile ${uboot};run setftpload;run flshb
[   27.531529] tftpk=run startusb;setenv loadfile ${linux};run setftpload;run flshkrnl
[   27.531597] tftpd=run startusb;setenv loadfile ${dualos};run setftpload;run flshdls
[   27.531665] tftpd2=run startusb;setenv loadfile ${dualos};run setftpload;run flshdls2
[   27.531734] tftpm=run startusb;setenv loadfile ${dualosmin};run setftpload;run flshdls
[   27.531804] tftpr=run startusb;setenv loadfile ${rfd};run setftpload;run flshrfd
[   27.881598] fatupdate=run clearenv;run bootup;run kernup;run dualosminup;run rfdup;setenv setnorloader ${setfatload};run handle_norfs
[   27.881701] tftpupdate=run clearenv;run tftpu;run tftpk;run tftpm;run tftpr;setenv setnorloader ${setftpload};run handle_norfs
[   27.881800] oldupdate=run fatupdate;setenv xtargs ${xtargs} update=fat;run usbrec1
[   27.881867] fastupdate=setenv linux uImage-fastboot;run fatupdate;setenv xtargs ${xtargs} update=fat;setenv linux uImage;run usbrec1
[   27.881970] fatld_uboot=run bootup
[   27.882003] fatld_kernel=run kernup
[   27.882037] fatld_dualosmid=run dualosup
[   27.882075] fatld_dualosmid2=run dualosup2
[   27.882114] fatld_dualosmin=run dualosminup
[   27.882153] fatld_rfd=run rfdup
[   27.882184] fatld_ffsmpt=run startusb;setenv setnorloader ${setfatload};run norfs_empty;run handle_norfs
[   27.882267] fatld_ffsfld=run startusb;setenv setnorloader ${setfatload};run norfs_filled;run handle_norfs
[   27.882351] fatld_ffskill=run norfs_default; run handle_norfs
[   27.882403] tftp_uboot=run tftpu
[   27.882435] tftp_kernel=run tftpk
[   27.882468] tftp_dualosmid=run tftpd
[   27.882502] tftp_dualosmid2=run tftpd2
[   27.882539] tftp_dualosmin=run tftpm
[   27.882573] tftp_rfd=run tftpr
[   27.882604] tftp_ffsmpt=setenv setnorloader ${setftpload};run norfs_empty;run handle_norfs
[   27.882677] tftp_ffsfld=setenv setnorloader ${setftpload};run norfs_filled;run handle_norfs
[   27.882750] tftp_ffskill=run norfs_default; run handle_norfs
[   27.882802] usbrecover=run setbootargs; update /
[   27.882845] usbdhcprecover=setenv xtargs ${xtargs} update=fab_dhcp; run usbrecover
[   27.882913] usbrec1=run startusb;run setbootargs;if fatload usb 0:1 0x85A00000 ${linux}; then setenv uid 0;elif fatload usb 1:1 0x85A00000 ${linux};then setenv uid 1; else run usbrecf; fi; run usbrec2
[   27.883063] usbrec2=if fatload usb ${uid}:1 0x85000000 ${ramdisk};then if fatload usb ${uid}:1 0x82000000 ${dualos};then run usbrec3; fi; fi; run usbrecf
[   27.883181] usbrec3=if bootm start 0x82000000; then setrtosaddr;if bootm loados; then startguestos;;bootm 0x85A00000 0x85000000; fi; fi; run usbrecf
[   27.883295] usbrecf=echo *** USB recovery download FAIL, stopping ***
[   27.883353] setbootargs=setenv bootargs console=${console},115200n8n mem=${linuxmem} maxcpus=${cores} root=/dev/${rootdev} rootwait lpj=1994752 panic=${panic} panic_on_oops=${panic_on_oops} usbcore.rh_oc_handler=1 ${xtargs}
[   27.883521] update=run clearbootconfig;run bootup;setenv bootcmd 'run fatupdate; run clearenv; setenv xtargs ${xtargs} update=fat;run setbootargs;run usbrec1';saveenv; reset
[   27.883652] clearresetcounter=mw ffffff14 0
[   27.883692] exitrecovery=setenv bootcmd ${bootcmd_default}; run clearresetcounter
[   27.883758] clearbootconfig=erase 0x400e0000 +1
[   27.883801] clearenv=protect off 0x40080000 +12288;erase 0x40080000 +12288;protect off 0x401C0000 +12288;erase 0x401C0000 +12288
[   27.883901] mrpropper=run exitrecovery;run clearbootconfig;run clearenv
[   27.883961] bootcmd_default=run setbootargs; if bootm start 0x40920000; then setrtosaddr; bootm loados; startguestos; fi; bootm
[   27.884060] stdin=serial
[   27.884086] stdout=serial
[   27.884112] stderr=serial
[   27.884139] console=ttyS0
[   27.884166] linuxmem=768M
[   27.884196]
[   27.884213] Environment size: 5891/131068 bytes
[   27.884661] NEMID # test
[   29.260318] NEMID #
[   29.260767] NEMID # help
[   42.597063] ?       - alias for 'help'
[   42.597102] base    - print or set address offset
[   42.597145] bdinfo  - print Board Info structure
[   42.597189] bootm   - boot application image from memory
[   42.597238] bootp   - boot image via network using BOOTP/TFTP protocol
[   42.597294] cmp     - memory compare
[   42.597330] coninfo - print console devices and information
[   42.597379] cp      - memory copy
[   42.597412] crc32   - checksum calculation
[   42.597451] echo    - echo args to console
[   42.597490] em_trace- display error memory entries in PRAM
[   42.597539] erase   - erase FLASH memory
[   42.597576] exit    - exit script
[   42.597610] false   - do nothing, unsuccessfully
[   42.597653] fatinfo - print information about filesystem
[   42.597701] fatload - load binary file from a dos filesystem
[   42.597752] fatls   - list files in a directory (default /)
[   42.597802] flinfo  - print FLASH memory information
[   42.597848] go      - start application at address 'addr'
[   42.597897] help    - print command description/usage
[   42.597943] imxtract- extract a part of a multi-image
[   42.597989] loop    - infinite loop on address range
[   42.598034] md      - memory display
[   42.598069] mii     - MII utility commands
[   42.598109] mm      - memory modify (auto-incrementing address)
[   42.598161] mtest   - simple RAM read/write test
[   42.598204] mw      - memory write (fill)
[   42.598242] nm      - memory modify (constant address)
[   42.598290] pci     - list and access PCI Configuration Space
[   42.598341] printenv- print environment variables
[   42.598385] protect - enable or disable FLASH write protection
[   42.598437] rarpboot- boot image via network using RARP/TFTP protocol
[   42.598493] reset   - Perform RESET of the CPU
[   42.598535] run     - run commands in an environment variable
[   42.598587] saveenv - save environment variables to persistent storage
[   42.598644] setenv  - set environment variables
[   42.598688] setrtosaddr- set the RAM entry address for the RTOS (autoselected based on the board ID)
[   42.598764] showvar - print local hushshell variables
[   42.598810] sleep   - delay execution for some time
[   42.598854] source  - run script from memory
[   42.598895] startguestos- start guest OS on CPU #2
[   42.598940] startsingleRTOS- start RTOS as single os from flash
[   42.598992] test    - minimal test like /bin/sh
[   42.599034] tftpboot- boot image via network using TFTP protocol
[   42.599087] true    - do nothing, successfully
[   42.599129] update  - perform a recovery update
[   42.599171] usb     - USB sub-system
[   42.599206] usbboot - boot from USB device
[   42.599245] version - print monitor version
```

## DualOS 

All around the system, there are references to `DualOS`. This reffers to the fact that system runs two operating systems at once. Linux which is mainly responsible for user interaction, and a separate RTOS that's probably tasked with dealing with CANbus and other time sensitive operations.

### Linux

As can be gleaned from Uboot, Linux kernel version is `Linux-2.6.34.13-02018-g843e5c6` and to the best of my knowledge all versions of lcn2kai are based around `2.6.34.13`, but compiled at different times and with slight differences. 
Things to note about Linux kernel are built in and loadable kernel modules, and there are a couple of unique ones. Most of the built in ones are related to RTOS communication:

```
kernel/drivers/virtio/virtio_nemid/virtio_shm.ko
kernel/drivers/virtio/virtio_nemid/virtio_mpf.ko
kernel/drivers/virtio/virtio_nemid/virtio_device.ko
kernel/drivers/virtio/virtio_nemid/virtio_test.ko
kernel/drivers/virtio/virtio_nemid/virtio_rfs.ko
kernel/drivers/char/errormem/errmem.ko
kernel/drivers/char/iosc/iosc_char.ko
```

RFS stands for , I guess, `root file system` and virtio kernel interface is used heavily. Driver `iosc_char.ko` implements a char device that's exposed in `/dev/iosc` which serves as a complex shared memory communication channel. 

Userspace interface to these communication channels seems to be libOSAL (OS Abstraction layer) and libiosc which also provide a shared memory interface used by Bosch processes via /dev/OSAL-Arena. More about this interface should be documented as we go along.


### RTOS - Triton

The other, real time , operating system is, as far as I can tell, refered to as `Triton` and seems to be based on some version of `T-Kernel` or `Tron` RTOS which stands to reason. Exploring this area is on my TODO. 



## System and processes

There are a couple of processes that run on the system that are of note, from UI , through IPC and tracing/debugging. 

### Bosch processes

Most important are the ones that support user interaction. All of them are located at `/opt/bosch/processes`:

```
:/opt/bosch/processes# ls
DAPIAPP.OUT                     libosal_linux_so.so          procbaselx_out.out       procphone_out.out
checksum_verify-noosal_out.out  libplayerengineclient_so.so  proccgs_out.out          procsds.out
gal_updater_out.out             libsortlib_so.so             prochmi_out.out          procsds_ts.out
libexitcatcher_so.so            libusbcontrol_so.so          procmapengine.out        procsmartphone_out.out
libgal_wrapper_so.so            libverticalkeyboard_so.so    procmedia_out.out        procsql_out.out
libiosclib_so.so                playerengine_out.out         procmediaplayer_out.out  procsxm_out.out
libipodcontrol_so.so            playerscript.sh              procmwlx_out.out         procvoice_out.out
```
The ones with `proc` prefix are started by init scripts and have fairly obvious names. We'll look into `procsmartphone_out`, which implements smart phone integration, later. 

These are very sensitive, and messing with any of them during runtime quickly causes a reboot. 

### Airbiquity 

In order to provide smartphone integration and internet connectivity, Bosch seems to have partnered with Airbiquity. Airbiquity's application endpoint can be found in `/opt/bosch/airbiquity` and it constist of a NodeJS application that facilitates all the fancy features like Apps, smart phone integration , internet connectivity and inter-process communication with the rest of lcn2kai system. It would do one good to piece together how it all works...

Notice that `Apps` menu on the head unit that lists Google, Yelp , Pandora or whatever as apps? That's actually rendering a web page from Airbiquity's middleware. More on that in a separate document.


### Trace framework

There appears to exist some sort of a tracing/debugging framework that is enabled on the system. It's supported by a couple of processes that are run by default:

```
# ps aux | grep trace
root        87  0.0  0.1   3676   916 ?        Sl   00:00   0:00 /bin/trace_syslog
root       122  0.1  0.0   3708   624 ?        Ssl  00:00   0:03 /bin/trace_be --port=7000 --server=172.17.0.5 --trccfg=/var/opt/bosch/dynamic/trace-data --com=/dev/ttyUSB0
root       522  0.0  0.1   3552   852 ?        S<   00:00   0:00 trace_tool -c 45578 -l 8
```

Process `trace_be`, for trace back end, is started on system startup and is constantly trying to connect to 172.17.0.5:7000 and if we start listening on that port on the laptop, it will actually start sending binary data. 
Trace backend can also log textual debug info whose verbosity can be controlled through init scripts and `etc` configs. This should be reversed and documented more thoroughly. 







