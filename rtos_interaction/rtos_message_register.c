#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "dynload.h"
#include "util.h"

// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi 
// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] ~/rtos_registry_testing.c -o ~/rtos_iosc_testing -ldl





int main(int argc, char **argv){

    if(argc != 8){

        printf("%s <dst_queue_name> <dst_queue_num> <service id> <maj version> <min version> <function id> <opcode>\n",argv[0]);
        printf("Example (registering for GyroData_UpdateUpReg (function 4 opcode 3) at mbx_10041 queue on service 11 (version 4 12))\n");
        printf("%s mbx_10041 10041 11 4 12 4 3\n",argv[0]);
        return 0;
    }

    int dst_queue = atoi(argv[2]);
    int dst_service = atoi(argv[3]);
    int svc_maj_version = atoi(argv[4]);
    int svc_min_version = atoi(argv[5]);
    int func_id = atoi(argv[6]);
    int opcode = atoi(argv[7]);
    int reg_id = 0;


    char buff[100];
    unsigned  int readbuff[2000];
    memset(readbuff,'\x00',2000);
    int pool_handle;

    if(dynload() != 0) return 1;
 

    //get the pool
    int b = OSAL_s32MessagePoolCreate(300000);
    printf("OSAL_s32MessagePoolCreate %d\n",b);
    b = OSAL_s32MessagePoolOpen();// should be 0 
    printf("pool current size: %d \n",OSAL_s32MessagePoolGetCurrentSize());


    //create our own msg queue
    int unk[4]= {0,0,0,0};
    int qc_handle = 0;
    int qc_result = OSAL_s32MessageQueueCreate("mbx_1337",100,0x38,4,&qc_handle);
    printf("%x  %x\n",qc_result,qc_handle);
    if(qc_result == -1){
        OSAL_s32MessageQueueOpen("mbx_1337",4,unk);
        qc_handle = unk[0];
    }   

    
    //open the target queue
    int result = OSAL_s32MessageQueueOpen(argv[1],4,unk);
    printf("Queue id %d %d %d\n",result,unk[0],unk[1]);
    int queueid = unk[0];

    //registration message
    int msg_handle = OSAL_s32MessageCreate(unk,26,2);
    printf("Message handle %x %x %x %x\n",unk[0],unk[1],unk[2],unk[3]);
    msg_handle = unk[1];
 
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)unk[0],msg_handle,4);
    message->unknown1 = 1337;
    message->queueNum = dst_queue;
    message->numBytes = 26;
    message->unknown2 = 2; 
    message->unknown3 =  0;
    message->msgType =  0x41;
    message->unknownWord4 =  0;
    message->queueSubId = 0;
    message->time = 0x4141;
    

    MessageDataStruct *p1 = pu32GetSharedBaseAdress();
    p1 = (MessageDataStruct*)((char*)p1+ (int)msg_handle * 0xc);
    memset(p1+39,0x00,30);
    p1->sender = 1337;
    p1->unknown2 = 0xffffffff;
    p1->receiver = dst_queue;
    p1->size = 26;
    p1->type = 0x41;
    p1->unknown3 = 2;
    p1->unknown4 = 0;
    p1->s_sub = 0;
    p1->d_sub = 0;
    p1->time = 0x1231;
    p1->serv_id = dst_service;
    p1->unknown5 = svc_maj_version; //4; //version number major
    p1->func_id = svc_min_version; //0xc;   //version number minor
    p1->opcode = 0;
    p1->x = 0xffff;
    p1->y = 0x112;
    //dump((char*)(p1),(p1->blocks*12));    
    short a[3] = {0,0,0};
    a[0] = msg_handle;
    // finally send the registration message
    result = OSAL_s32MessageQueuePost(queueid,unk,0x8,0);
    printf("%d\n",result);



    int x[10] = {0};
    int y[10] = {0};
    int z[10] = {0};
    int first = 1; // we want to reply to status update

    //listen for updates forever
    while(1==1){ 
    
    memset(x,0,sizeof(x));
    memset(z,0,sizeof(z));
    memset(y,0,sizeof(y));
    
    int w = 0; // if something is wrong, don't block indefinitely
    while(OSAL_s32MessageQueueWait(qc_handle,(int**)&x,8,0,1000)<1)
    {   
        if(w == 4) break;
        w++;
    }
        
    // we recieved a message in our queue
    unsigned int msg_handle = (unsigned int)x[1];
    MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)x[0],msg_handle,4);
    if(!message) continue;
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
    // get the message data
    MessageDataStruct *p = pu32GetSharedBaseAdress();
    p = (MessageDataStruct*)((char*)p+ (int)msg_handle * 0xc);

    // it seems like all valid messages should start with 0xdaca
    //dump((char*)(p),message->numBytes);    printf("\n\n");
    //mgk:0x10daca unk2: 0xffffffff #blk:42 sndr:10041 rcvr: 265 sz:476 unk3:0x02 unk4:0x0 typ:69 s_sub:0x00, d_sub:0x00, time:94308 serv_id:11, unk5:0x06, fn:65 op:6
   // if(p->func_id != 65 || p->opcode != 6 || p->serv_id != 11) continue;
    printf("mgk:0x%04x unk2: 0x%04x #blk:%d sndr:%d rcvr: %d sz:%d unk3:0x%02x unk4:0x%01x typ:%d s_sub:0x%02x, d_sub:0x%02x, time:%d serv_id:%d, unk5:0x%02x, fn:%d op:%d\n"
             ,p->magic
             ,p->unknown2
             ,p->blocks
             ,p->sender
             ,p->receiver
             ,p->size
             ,p->unknown3
             ,p->unknown4
             ,p->type
             ,p->s_sub
             ,p->d_sub
             ,p->time
             ,p->serv_id
             ,p->unknown5
             ,p->func_id
             ,p->opcode);
    reg_id = p->unknown5;

    dump((char *)p,(p->blocks*12));
    printf("===============================\n");
    dump((char*)p+40,(p->blocks*12)-52); // just the message data , sans header

    
    if(!first && p->opcode == 6 && p->func_id == 4){ // well, we know the struct for Gyro Data, sort of...
    for(int i = 0; i < p->y;i++){ // for function id 4
            int16_t x = *(((int16_t *)p)+((48+i*10 )+4)/2);
            printf("%hd ",x );
            dump(((char *)p)+(48+i*10 ),10);    

        }
    }
    /*if(!first && p->opcode == 6 && p->func_id == 65){ // similar as above, but for Gyro3D data
            for(int i = 0; i < p->y;i++){ // for function id 4
    dump(((char *)p)+(52+i*22 ),18);    
        }
    }
    */
    
    fflush(0); // if i'm logging over ssh, i want to see flush stdout right away

    if(first){ // first message, send registration
        int msg_handle = OSAL_s32MessageCreate(unk,32,2);
        //printf("Message handle %x %x %x %x\n",unk[0],unk[1],unk[2],unk[3]);
        msg_handle = unk[1];
     
        MessageStruct *message = OSAL_pu8MessageContentGet((unsigned int*)unk[0],msg_handle,4);
            message->unknown1 = 1337;
            message->queueNum = dst_queue;
            message->numBytes = 32;
            message->unknown2 = 2; 
            message->unknown3 =  0;
            message->msgType =  0x45; 
            message->unknownWord4 =  0;
            message->queueSubId = 0;
            message->time = p->time+5;
            MessageDataStruct *p1 = pu32GetSharedBaseAdress();
         p1 = (MessageDataStruct*)((char*)p1+ (int)msg_handle * 0xc);

         p1->sender = 1337;
         p1->receiver = dst_queue;
         p1->size = 32;
         p1->type = 0x45;
         p1->s_sub = 0;
         p1->d_sub = 0;
         p1->time = p->time+5;
         p1->serv_id = dst_service;
         p1->func_id =  func_id;
         p1->unknown5 = reg_id; //registration id
         p1->opcode =  opcode; //3 == register, 4 unregister ? 
          p1->x = 0;
         p1->y = 0;

        short a[3] = {0,0,0};
        a[0] = msg_handle;
        result = OSAL_s32MessageQueuePost(queueid,unk,0x8,0);

        printf("%d\n",result);
        first = 0;
        sleep(1); // wait a bit before resuming listening

        }
    }
    // if something is wrong and infinite loop breaks out, try to clean up
    int c  = OSAL_s32MessageQueueDelete("mbx_1337");
    printf("Delete %d\n",c);
    c  = OSAL_s32MessageQueueClose(qc_handle);
    printf("Close %d\n",c);

    cleanup();
    return 0;

}    

