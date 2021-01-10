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

The intended use for the exposed USB port is for either for loading gigabytes of MP3s for those long drives, for playing your Spotify favorites via your smartphone or simply as a usb charger. That being said, it would be interesting to know if the headunit supports has drivers for and supports any additional devices. Usuall trick is to simply plug in a keyboard in hopes it would give you console access. We aren't so lucky in this case, but maybe the developers left something else that could be useful. We could try and plug in random USB devices that we have, but that list is usually relatively short. A more automated way to do this would be to use Facedancer and umap2. 

## Getting dirty 
### Finding serial console
### Root via u-boot & ssh
## An accessible vulnerability
## Non-invasive exploit and enabling remote shell
## Where from here

