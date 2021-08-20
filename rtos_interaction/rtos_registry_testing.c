
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynload.h"
#include "util.h"

// this test program demonstrates how to use functions from OSAL to enumerate registry 
// 
// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi 

// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] ~/rtos_registry_testing.c -o ~/rtos_iosc_testing -ldl

// tends to lead to a crash which can also bring down the whole OS, probably not cleaning up something 


int main(int argc, char **argv){


   char buff[100];
   unsigned  int readbuff[2000];
   int pool_handle;
   memset(readbuff,'\x00',2000);


    if(argc != 3){
        printf("Usage example: %s /dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT MAXPOOLSIZE\n",argv[0]);
        return 1;
    }

    if(dynload() != 0) return 1;


    int fp = OSAL_IOOpen(argv[1],4);
    if(fp == 0xffffffff) {
        printf("No such device: %s\n",argv[1]);
    cleanup();
    return 1;
    }

    printf("listing registry key: %s\n",argv[1]);
    
   // get sub path
    ulong b1[2];
    char b2[200];
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    int b = 0 ; 
    printf("subkey:\n");
    while ( b =  OSAL_s32IOControl(fp,5,(int*)&readbuff) != -1){
    printf("%s\n",(char*)&readbuff[2]);
    }
 
    
    // get value name
    printf("value names: \n");
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    while( b = OSAL_s32IOControl(fp,2,(int*)&readbuff) != -1){
    printf("%s\n",(char*)&readbuff[2]);
    }
    
    //query value data
    readbuff[0] = (unsigned int)argv[2];
    readbuff[1] = 200;
    readbuff[2] = 0;
    readbuff[3] = (unsigned int)&b2;
     b = OSAL_s32IOControl(fp,1,(int *)&readbuff);
    printf("value read retval %d %d %d %ld\n",b,readbuff[2],b2[0],b1[1]);
    printf("data: %s", b2); // some key values are actually strings


    OSAL_s32IOClose(fp);

    cleanup();
}    

