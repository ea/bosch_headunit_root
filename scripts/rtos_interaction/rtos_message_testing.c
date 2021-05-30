
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dynload.h"
#include "util.h"
// this test program demonstrates how to use functions from OSAL to enumerate registry 
// 
// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi 

// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] ~/rtos_registry_testing.c -o ~/rtos_iosc_testing -ldl

// tends to lead to a crash which can also bring down the whole OS, probably not cleaning up something 

// list of queues:
// mbx_%d 
// KDS_MSGQ LI_PRM_REC_MQ PRM_MSGQUEUE  OSAL_CB_HDR_LI_MAIN NOIOSC_CB_HDR_LI_%d 
// OSAL_CB_HDR_TE TE_TERM_MQ LI_TERM_MQ FFD_MSGQ FFD_MSQL KDS_MSQL





int main(int argc, char **argv){


   char buff[100];
   unsigned  int readbuff[2000];
   int pool_handle;
   memset(readbuff,'\x00',2000);

    if(argc != 2){
        printf("Usage example: %s mbx_0\n",argv[0]);
        return 1;
    }

    if(dynload() != 0) return 1;
 
    int b = OSAL_s32MessagePoolCreate(300000);
    printf("OSAL_s32MessagePoolCreate %d\n",b);

    OSAL_s32MessagePoolOpen();// should be 0 
    printf("pool current size: %d \n",OSAL_s32MessagePoolGetCurrentSize());
    int unk[2]= {0,0};
    int result = OSAL_s32MessageQueueOpen(argv[1],4,unk);
    printf("Queue id %d %d %d\n",result,unk[0],unk[1]);
    int queueid = unk[0];

    int x[10] = {0};
    int y[10] = {0};
    int z[10] = {0};
    while(1==1){
    memset(x,0,sizeof(x));
    memset(z,0,sizeof(z));
    memset(y,0,sizeof(y));
    result = OSAL_s32MessageQueueWait(queueid,(int**)&x,8,(int**)&y,(int**)&z);

    unsigned int *msg_handle = (unsigned int*)x[1];
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)x[0],msg_handle,4);

    printf("unk1:%04x queue#:%04x, bytes:%08x unk2:%04x unk3:%02x msgType:%02x unk4:%04x qSubId:%04x time:%08x\n"
        ,message->unknown1
        ,message->queueNum
        ,message->numBytes
        ,message->unknown2
        ,message->unknown3
        ,message->msgType
        ,message->unknownWord4
        ,message->queueSubId
        ,message->time);
    char *p = pu32GetSharedBaseAdress();
    p = p+ (int)msg_handle * 0xc;

    // it seems like all valid messages should start with 0xdaca
    dump((char*)(p),message->numBytes);    printf("\n\n");

}


    cleanup();
    return 0;

}    

