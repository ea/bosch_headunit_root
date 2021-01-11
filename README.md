# Rooting Bosch lcn2kai Headunit

<insert rooted unit pic>
  

My Nissan Xterra came with a (for the time) modern head unit that has a touch screen, built-in navigation, backup camera display, multimedia features and smartphone connectivity. Some of the more advanced features are only available through NissanConnect App which requires registration and subscription. I've never used it and I'm not even sure if it's still supported. 

Wouldn't it be neat if were able to get code execution on the device and even develop extensions and apps of our own? 

One of the features I wanted to see is a simple GPS data logger and that idea will serve as guidance and goal in this reverse engineering and rooting endeavor. In this writeup, I present my approach to analyzing the system, enumerating possible attack surface, getting a shell via physical access and leveraging that to come up with an exploit that **doesn't require taking your dashboard apart** and results in a clean **root access over ssh**. 

I will share the code to reproduce this on your vehicle and a sample application that achieves the GPS data logging goal. However, here **you will find ABSOLUTELY NO information about bypassing any DRM/copyright mechanisms**, especially related to navigation maps and any online services. Moverover, any code or instructions herein come with no warranty and you follow them at your own risk. 

## Cars running lcn2kai

I would like to build an expanded list of headunits and car makes/models/years that are affected by the bug we'll abuse to gain root shell access with. So far, that list includes numerous Nissan models from around 2015 onward including Xterra, Rogue, Sentra, Altima, Frontier as well as some other Nissan comercial vehicles. Given that the head unit in question is manufactured by Bosch, chances are that the same platform can be found in many other vehicles. 

Put simply, if your headunit looks similar to the photo above, chances are that this exploit (or a similar variation) would work. 

So far, I've only tested this on my 2015 Nissan Xterra. 

Internally (and partially externally) Bosch and Nissan seem to reffer to this head unit version as lcn2kai which is how I'll be addressing it in these docs. If anybody happens to know what it stands for, I'd like to know.

## Attack surface

Ultimately, I'd like to be able to gain root on my car without having to use a screwdriver. I'm no stranger to taking things apart but have an odd habit of ending up with extra parts after reassembly. That will basically guide where I'll be looking for vulnerabilities first. 

Looking at it, as installed in a car, headunit's interaction with the outside world comes either through a USB port in the handrest/console or via bluetooth connectivity. USB port is meant for additional multimedia content served via usb flash drive or a smartphone and bluetooth connectivity can be used to interact with a smartphone (playing music, handsfree calls , NissanConnect app...). Ideally, I'll be looking for a code execution vulnerability that can be triggered through either of these as it could be performed in-place, without removing the head unit from the dash board. 

### Secret menu

A "secret" operator menu can be accessed by pressing and holind "App" button, then rotating the "TUNE" knob anti-clockwise a couple of clicks, then clockwise , then anticlockwise again. When this cheat code is entered, a new menu is opened that has several diagnostics options, head unit version information as well as a firmware update option.

<insert menu photo>

### Firmware update

Writeups on jailbreaking and rooting other car infotainment systems and headunits (like this excellent writeup on [jailbreaking Subaru Starlink](https://github.com/sgayou/subaru-starlink-research/blob/master/doc/README.md) ) have always paid special attention to firmware upgrades because that's when the systems are most exposed. 

There have been various versions of firmware updates for lcn2kai shared on random online forums that were obtained one way or the other. Head unit I'm working on haven't been updated since it was sold, parts relevant to this writeup remain unchanged on all versions. 

Firmware update is performed by first plugging in a USB stick that contains firmware update images and then entering the secret menu, selecting the update/upgrade option and following instructions. When the upgrade process is initiated, the system reboots into an upgrade mode, performs thorough cryptographic validation on supplied firmware upgrade files and only then proceeds to copy/flash/update. 

<insert firmware upgrade photos>

While firmware files and packages aren't encrypted, they are properly digitaly signed. Every component of the firmware upgrade (kernel, boot loader, linux file system...) is hashed and signed via standard public key cryptography algorithms and the whole scheme looked solid. I cautiously want to stay clear of trying to break any DRM, so I decided to move on with a bookmark to return to this process if all else fails. 

### Enumerating USB devices

The intended use for the exposed USB port is for either for loading gigabytes of MP3s for those long drives, for playing your Spotify favorites via your smartphone or simply as a usb charger. That being said, it would be interesting to know if the headunit supports has drivers for and supports any additional devices. Usuall trick is to simply plug in a keyboard in hopes it would give you console access. 

One easy trick to check if keyboard is supported at all (if the underlying system is Linux) is to issue a [Magic SysRq Key](https://en.wikipedia.org/wiki/Magic_SysRq_key) sequence to try and reboot the system. This actually does work on lcn2kai, but it turns out that the keyboard isn't connected to the console at all. So while kernel does process SysRq keys, it's otherwise useless. 

We could try and plug in random USB devices that we have, but that list is usually relatively short. A more automated way to do this would be to use [Facedancer and umap2](https://github.com/nccgroup/umap2). Facedancer can emulate diferent USB devices so one can simply cycle through different VID and PID values to detect which devices are supported by the host. Umap2 comes with a script called `umap2vsscan` which does just that:

```
umap2vsscan -P fd:/dev/ttyUSB0 -d $UMAP2_DIR/data/vid_pid_db.py
```

This would scan through a huge list of known VID and PID combinations and will print out the corresponding device (and Linux driver from which VID/PID came from). Since this can take quite a while, we can filter out this list a bit to focuse on what's interesting to us. There's basically two types of devices I'd be interested in finding support for: a TTL2USB adapter (like FTDI serial cables) that would somehow be connected to serial console by udev or something, or a USB network adapter that would hopefully be autoconfigured, or would take an address via DHCP. Editing the list that comes with umap2 to only contains those leaves us with about 1000 devices, which is acceptable and doable in reasonable time, just be careful no to kill your car battery while waiting for this to finish. 

With patience , we get the following result:

```
[ALWAYS] Found 1 supported device(s) (out of 1098):
[ALWAYS] 0. vid:pid 077b:2226, vendor: Linksys, product: USB200M 100baseTX Adapter,
driver: drivers/net/usb/asix_devices.c, info: device not reached set configuration state
```

This is great news. Driver `asix_devices.c` is used by a very very common USB ethernet adapter. Chances are your local electronics store has one randomly branded for about $10. I found this one that suits the purpose:

<insert photo of usb ethernet adapter>

One of the ways to make sure, before purchasing, the usb ethernet adapter indeed uses `asix_devices` driver is to search the manufacturer's website for Linux drivers. Although mostly useless because every linux installation will already come with it built in, you can often find a tarball that contains the driver the device uses. 

Now the question is if it will actually work. Plug the usb ethernet adapter into car's USB port, connect it via ethernet cable to your laptop and observe the LEDs turn on! The device seems to be initialized, but there are no DHCP requests. Sniffing the network with Wireshark, though, reveals that something is sending packets from IP `172.17.0.1`. Further more, it seems to be sending out TCP SYN packets to port 7000 with destination address `172.17.0.5`. It seems like the lcn2kai expect the other end of ethernet connection to have `172.17.0.5` assigned and indeed configuring laptop's ethernet port with this static IP seems to work. Starting up netcat to listen on port 7000 and it will get a connection from lcn2kai which will send a bunch of binary data. It will later be revealed that this is part of a tracing framework, possibly used to aid debugging during development. 

At this point, I was hoping to find some sort of server listening on some port that would serve as a good attack point, but no such luck. Scanning the device via Nmap reveals that all but one port are blocked by firewall. That one port is 22, for SSHd, but even though it's not blocked by firewall, nothing is listening on it. This is good news, though. It means we'll have an easy way of connecting to the system once we enable SSH server. 

## Getting dirty 

I've explored some other, obvious, venues of attack in hope of striking gold randomly. These included various attempts at command injection via file system names, phone bluetooth IDs , special files on the drives... None of it worked immediately. 

### Finding serial console
### Root via u-boot & ssh
## An accessible vulnerability
## Non-invasive exploit and enabling remote shell
## Where from here

