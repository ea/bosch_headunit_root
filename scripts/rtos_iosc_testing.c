#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


// this test program demonstrates how to use functions from OSAL to
// for Intra-OS communication
// you can open devices on the RTOS side (/dev/kds for ex.) and read stuff
// from specified constants

// TODO: figure out message passing interface and get other more interesting data
// lcn2kai env doesnt have gcc , obviously, so this has to be cross compiled and then copied over
// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi

// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] rtos_iosc_testing.c -o rtos_iosc_testing -ldl
// then run like:
// ./rtos_iosc_testing 0xdfe 0x18 /dev/kds 
// tends to lead to a crash which can also bring down the whole OS, probably not cleaning up something

void hexdump(char *p, int size) {
	for (int i = 0; i < size; i++) {
		if(i && !(i % 16)) printf("\n");
		printf("%02x", p[i]);
	}
	printf("\n");
}

void printstrings(char *p, int size) {
	for (int i = 0; i < size; i++) {
		if(i && !(i % 16)) printf("\n");
		if (p[i] >= 0x21 && p[i] <= 0x7e) {
			printf("%c", p[i]);
		} else {
			printf(".");
		}
	}
	printf("\n");
}

int main(int argc, char **argv){

	void *handle;
	union {
		char rawdata[1024];
		struct {
			unsigned short code;
			unsigned short len;
			unsigned short unknown;
			char data[1012];
		} ioread;
	} buffer;
	int (*OSAL_IOOpen)(const char*,int param);
	void (*OSAL_s32IOClose)(int fp);
	void (*OSAL_s32IORead)(int fp,char *buff,int code);
	handle = dlopen("/opt/bosch/processes/libosal_linux_so.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "Error: %s\n", dlerror());
		return EXIT_FAILURE;
	}
	*(void**)(&OSAL_IOOpen) = dlsym(handle, "OSAL_IOOpen");
	*(void**)(&OSAL_s32IOClose) = dlsym(handle, "OSAL_s32IOClose");
	*(void**)(&OSAL_s32IORead) = dlsym(handle, "OSAL_s32IORead");

	if (!OSAL_IOOpen || !OSAL_s32IOClose || !OSAL_s32IORead) {
		fprintf(stderr, "Error: %s\n", dlerror());
		dlclose(handle);
		return EXIT_FAILURE;
	}

	int fp = OSAL_IOOpen(argv[3],4);
	if (fp == 0xffffffff) {
	    printf("No such device: %s\n",argv[3]);
		dlclose(handle);
		exit(0);
	}
	//bReadEntry(this,0xdfe,0x18,(uchar *)(this + 5));
	unsigned short word1 = (unsigned short)strtol(argv[1],0,16);
	unsigned short word2 = (unsigned short)strtol(argv[2],0,16);
	for (; word1 < 0xffff; word1++){
		memset(buffer.rawdata, 0, sizeof(buffer));
		buffer.ioread.code = word1;
		buffer.ioread.len = word2;

		OSAL_s32IORead(fp, buffer.rawdata, 0xf6);
		if (buffer.ioread.code == 0xffff) continue;

		printf("\n%04x %04x %04x:\n", buffer.ioread.code, buffer.ioread.len, buffer.ioread.unknown);
		hexdump(buffer.ioread.data, buffer.ioread.len);
		printstrings(buffer.ioread.data, buffer.ioread.len);
	}
	OSAL_s32IOClose(fp);
	dlclose(handle);
}
