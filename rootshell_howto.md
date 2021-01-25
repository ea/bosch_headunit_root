# Rootshell How-To

The main writeup goes over system overview and the whole discovery process. This is a shorter guide that should be easy to follow. 

#### Table Of Contents

- [Preparing the flash drive](#preparing-the-flash-drive)
- [Connecting via network](#connecting-via-network)
- [Testing on other models](#testing-on-other-models)

## Preparing the flash drive

As discussed in the full  writeup, to gain a remote root shell on the head unit, we can abuse the arbitrary mount vulnerability. In short, this vulnerability lets us replace certain system commands with ones of our choosing. 
To do so , we must prepare a flash drive with a special file system. Script `rootshellfs.sh` will create an `ext2` file system image of 8mb and copy `logger` script to it. The created filesystem will have it's UUID set to empty and FS label set to `../../usr/bin/` which is what we need for exploit to work. You should see something like:

```
$ ./rootshellfs.sh
8192+0 records in
8192+0 records out
8388608 bytes (8.4 MB, 8.0 MiB) copied, 0.0269391 s, 311 MB/s
mke2fs 1.45.5 (07-Jan-2020)
Discarding device blocks: done
Creating filesystem with 2048 4k blocks and 2048 inodes

Allocating group tables: done
Writing inode tables: done
Writing superblocks and filesystem accounting information: done

======================================
Rootshell file system prepared. Insert flash drive and do:
$ sudo dd if=./rootshell.ext2 of=/dev/sd#
Make sure to replace sd# with your actuall flash drive and don't overwrite your system drive!!!
```

Above will create a file called `rootshell.ext2` which must then be written to your flash drive directly (not copied to existing file system). NOTE: This will effectively erase any data that is present on the drive, so make sure it's empty. Easy way to write this image to flash drive is via `dd`, as the shell script instructs:

```
$ sudo dd if=./rootshell.ext2 of=/dev/sdb
16384+0 records in
16384+0 records out
8388608 bytes (8.4 MB, 8.0 MiB) copied, 0.0208436 s, 402 MB/s
```
That's it. The flash drive is prepared. Eject it and go to your car. 

Next step is to actually execute the exploit, which is very simple. Turn on the head unit (the car doesn't have to actually run, key in ON position). Wait till the system boots up and navigation or radio starts and simply insert the USB flash drive. 

If everything went according to plan, you should see the lcn2kai reboot after about 10 seconds. 

Make sure to unplug your usb flash drive before the system boots up again! If the whole thing worked, when you open the flash drive on your computer again (it's ext2 file system, so linux required), besides `logger` script and `lost&found` you should see a file named `it_worked` with kernel info in it:

```
$ cat /media/user/.._.._usr_bin_/it_worked
Linux (none) 2.6.34.13-02018-g843e5c6 #1 SMP PREEMPT Thu May 15 16:58:54 IST 2014 armv6l GNU/Linux
```

That really confirms that code execution has worked and that SSHd should be enabled on the car's head unit. 

## Connecting via network

Now that the usb flash exploit has been executed, the SSH daemon should be enabled and we should be able to connect to the head unit remotely. There are only two requirements, a specific network adapter and static IP config. As the main writeup explains, only a limited number of devices are supported. If you don't already have one, you will need to obtain a USB Ethernet adapter that's based on ASIX `AX88772B` controller. These are fairly common and I've used this generic one branded as `Manhattan USB 2.0 Fast Ethernet Adapter` which should be around $10. Whichever one you get, test it out on your linux machine to make sure it has the correct controller. You should see something like the following in `dmesg` output:

```
[388457.942658] usb 3-2: New USB device found, idVendor=0b95, idProduct=772b, bcdDevice= 0.01
[388457.942663] usb 3-2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[388457.942666] usb 3-2: Product: AX88772B
[388457.942669] usb 3-2: Manufacturer: ASIX Elec. Corp.
[388457.942671] usb 3-2: SerialNumber: 000006
[388458.965005] asix 3-2:1.0 eth0: register 'asix' at usb-0000:00:14.0-2, ASIX AX88772B USB 2.0 Ethernet, 00:6f:00:01:24:09
[388458.965145] usbcore: registered new interface driver asix
[388458.990310] asix 3-2:1.0 enx006f00012409: renamed from eth0
```

Next , you'll need to configure ethernet interface on your laptop to have a static IP address. By default, `lcn2kai` is configured with address `172.17.0.1` and netmask 255.255.0.0 , so on laptop side we want 172.17.0.5 as a static IP (other lcn2kai services expect debug system to be at .5 ). When you have the static IP set up, plug the USB Ethernet adapter into car's USB port and connect it via ethernet cable to your laptop. Turn on the system and once it's fully booted up (it might take a while for all of init.d to be done and sshd actually starts) you should be able to simply ssh into lcn2kai:

```
$ ssh root@172.17.0.1
!! The root file system is READ-ONLY !!
root@(none):~# uname -a
Linux (none) 2.6.34.13-02018-g843e5c6 #1 SMP PREEMPT Thu May 15 16:58:54 IST 2014 armv6l GNU/Linux
root@(none):~# cat /
/                     /etc/                 /media/               /rfs_version.txt      /usr/
/Settings/            /home/                /mnt/                 /sbin/                /var/
/bin/                 /include/             /share/
/boot/                /lcn2kai_version.txt  /opt/                 /shared/
/cc_label.txt         /lib/                 /proc/                /sys/
/dev/                 /lost+found/          /rfs2/                /tmp/
root@(none):~# cat /lcn2kai_version.txt
AI_PRJ_NISSAN_LCN2KAI_13.14V12
root@(none):~#
```

Now you can look around, edit configuration files and make yourself more comfortable. 

## Testing on other models

Let's assume you have a head unit model that's similar to lcn2kai, or an lcn2kai that's a different version. It would be nice if we could run a simple test, that doesn't modify anything on the system, that would tell us if the same exploit works. Script `testfs.sh` does the same thing as `rootshellfs.sh` but ommits any changes to the file system. Instead , it exploits the same vulnerability but only to create a `it_worked` file on the flash drive. 

So, to first test the exploitability, use `testfs.sh` script to create a test file system to write to your USB flash drive, plug it into your car as described above and then check if the system rebooted and if the flash drive now contains `it_worked` file with kernel version. 

Of course, things could fail for numerous reasons, but this should be a good starting point. For one, even if same udev scripts are vulnerable to dir traversal when mounting a flash drive, `logger` might not be the next command, so some debugging could be required. 


