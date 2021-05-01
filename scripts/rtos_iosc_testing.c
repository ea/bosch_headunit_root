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

void hexdump(char *p, int size){
for ( int i = 0; i< size; i++ )
{
if((i % 16)==0) printf("\n");

   printf("%02x", p[i]);
}

}

void printstrings(char *p, int size){

	int total_printed = 0;
	int printed = 0;
	while(total_printed < size){
		printed = printf("%s",p+total_printed);
		if(printed == 0){printed = 1;}else{printf("\n");}
		total_printed += printed;
	}
}

main(int argc, char **argv){


   void *handle;
   char buff[100];
   unsigned short int readbuff[2000];
   memset(readbuff,'\x00',2000);
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

    if (!OSAL_IOOpen) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }
    if (!OSAL_s32IOClose) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }

    if (!OSAL_s32IORead) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }

    int fp = OSAL_IOOpen(argv[3],4);
    if(fp == 0xffffffff) {
    	printf("No such device: %s\n",argv[3]);
    	exit(0);
    }
    //bReadEntry(this,0xdfe,0x18,(uchar *)(this + 5));
    for(unsigned short int i = 0 ; i < 0xffff; i++){
    unsigned short int word1 = (unsigned short int)strtol(argv[1],0,16);
    unsigned short int word2 = (unsigned short int)strtol(argv[2],0,16);
    readbuff[0] = word1+i;
    readbuff[1] = word2;



    OSAL_s32IORead(fp,&readbuff,0xf6);
    int empty = 1;
    for(int j = 8; j < word2;j++){
    	if(readbuff[j] != '\x00') {empty = 0; break;}

    }
    if(empty)continue;
    if((unsigned short int)*readbuff == 0xffff)continue;
    printf("0x%x:\n",word1+i);
    hexdump(readbuff,word2);
    printstrings(readbuff,word2);
       memset(readbuff,'\x00',2000);

	}
    OSAL_s32IOClose(fp);
        gets(buff);
}
