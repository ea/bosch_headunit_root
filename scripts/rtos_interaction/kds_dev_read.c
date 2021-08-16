#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "util.h"
#include "dynload.h"

// this test program demonstrates how to use functions from OSAL to
// for Intra-OS communication
// you can open devices on the RTOS side (/dev/kds for ex.) and read stuff
// from specified constants

// TODO: figure out message passing interface and get other more interesting data
// lcn2kai env doesnt have gcc , obviously, so this has to be cross compiled and then copied over
// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi
// on debian install package gcc-arm-linux-gnueabi

// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] rtos_iosc_testing.c -o rtos_iosc_testing -ldl
// then run like:
// ./rtos_iosc_testing 0xdfe 0x18 /dev/kds 
// tends to lead to a crash which can also bring down the whole OS, probably not cleaning up something


int main(int argc, char **argv){

	union {
		char rawdata[1024];
		struct {
			unsigned short code;
			unsigned short len;
			unsigned short unknown;
			char data[1012];
		} ioread;
	} buffer;

	if (argc < 4) {
		printf("usage: %s <code> <len> <dev> [loop]\n", argv[0]);
		return EXIT_FAILURE;
	}

    if(dynload() != 0) return 1;


	int fp = OSAL_IOOpen(argv[3],4);
	if (fp == 0xffffffff) {
	    printf("No such device: %s\n",argv[3]);
		return EXIT_FAILURE;
	}

	unsigned short word1 = (unsigned short)strtol(argv[1],0,16);
	unsigned short word2 = (unsigned short)strtol(argv[2],0,16);
	for (; word1 < 0xffff; word1++){
		memset(buffer.rawdata, 0, sizeof(buffer));
		buffer.ioread.code = word1;
		buffer.ioread.len = word2;

		OSAL_s32IORead(fp, buffer.rawdata, 0xf6);
		if (buffer.ioread.code == 0xffff) continue;

		printf("\n%04x %04x %04x:\n", buffer.ioread.code, buffer.ioread.len, buffer.ioread.unknown);
		dump(buffer.ioread.data, buffer.ioread.len);

		if (argc < 5) break;
	}
	OSAL_s32IOClose(fp);
	cleanup();
	return EXIT_SUCCESS;
}
