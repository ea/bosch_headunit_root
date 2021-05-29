
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


// this test program demonstrates how to use functions from OSAL to enumerate registry 
// 
// to compile it , you'll need an extracted root fs (or at least the needed binaries) and arm eabi 

// compile like so:
// arm-linux-gnueabi-gcc   --sysroot=[PATH_TO_ROOTFS_COPY] -B [PATH_TO_ROOTFS_COPY] ~/rtos_registry_testing.c -o ~/rtos_iosc_testing -ldl

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
   unsigned  int readbuff[2000];
   int pool_handle;
   memset(readbuff,'\x00',2000);
    int (*OSAL_IOOpen)(const char*,int param);

    void (*OSAL_s32IOClose)(int fp);
    void (*OSAL_s32IORead)(int fp,char *buff,int code);
    int (*OSAL_s32IOControl)(int fp,unsigned int f,char *buff);
    int (*OSAL_s32MessagePoolCreate)(ulong size);
    int (*OSAL_s32MessagePoolOpen)();
    int (*OSAL_s32MessagePoolGetCurrentSize)();
    void (*OSAL_vPrintMessageList)( int fd);
    void (*vOsalTraceOutRegistry)(char *path);
    int (*OSAL_pu8MessageContentGet)(unsigned int *p1,unsigned int *p2,unsigned int code);
    int (*OSAL_s32MessageQueueOpen)(char *queuename,int x, int *unknown);
    int (*OSAL_s32MessageQueueStatus)(int queueid,int *x, int *y);
    int (*OSALUTIL_prOpenDir)(const char*);
    int (*OSALUTIL_prReadDir)(int *);
    int (*OSAL_s32MessageQueueWait)(int queueid,int *x, int i,int *y,int *z);
    int (*s32ReadCompleteMsg)(int msg,int x);
    int (*u32GetActualMsg)(int msg);
    int (*OSAL_s32MessageDelete)(int msg_type,int msg_handle);
    void * (*pu32GetSharedBaseAdress)();

    if(argc != 3){
        printf("Usage example: %s /dev/registry/LOCAL_MACHINE/SOFTWARE/BLAUPUNKT MAXPOOLSIZE\n");
    }

    handle = dlopen("/opt/bosch/processes/libosal_linux_so.so", RTLD_LAZY);
        if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    *(void**)(&OSAL_IOOpen) = dlsym(handle, "OSAL_IOOpen");
    *(void**)(&OSAL_s32IOClose) = dlsym(handle, "OSAL_s32IOClose");
    *(void**)(&OSAL_s32IORead) = dlsym(handle, "OSAL_s32IORead");
    *(void**)(&OSAL_s32IOControl) = dlsym(handle,"OSAL_s32IOControl");
    *(void**)(&OSAL_s32MessagePoolCreate) = dlsym(handle,"OSAL_s32MessagePoolCreate");
    *(void**)(&OSAL_s32MessagePoolOpen) = dlsym(handle,"OSAL_s32MessagePoolOpen");
    *(void**)(&OSAL_s32MessagePoolGetCurrentSize) = dlsym(handle,"OSAL_s32MessagePoolGetCurrentSize");
    *(void**)(&OSAL_vPrintMessageList) = dlsym(handle,"OSAL_vPrintMessageList");
    *(void**)(&OSAL_pu8MessageContentGet) = dlsym(handle,"OSAL_pu8MessageContentGet");
    *(void**)(&vOsalTraceOutRegistry) = dlsym(handle,"vOsalTraceOutRegistry");
    *(void**)(&OSAL_s32MessageQueueOpen) = dlsym(handle,"OSAL_s32MessageQueueOpen");
    *(void**)(&OSAL_s32MessageQueueStatus) = dlsym(handle,"OSAL_s32MessageQueueStatus");
    *(void**)(&OSALUTIL_prOpenDir) = dlsym(handle, "OSALUTIL_prOpenDir");
    *(void**)(&OSALUTIL_prReadDir) = dlsym(handle, "OSALUTIL_prReadDir");
    *(void**)(&OSAL_s32MessageQueueWait) = dlsym(handle,"OSAL_s32MessageQueueWait");
    *(void**)(&s32ReadCompleteMsg) = dlsym(handle,"OSAL_s32MessageQueueWait");
    *(void**)(&u32GetActualMsg) = dlsym(handle,"u32GetActualMsg");
    *(void**)(&OSAL_s32MessageDelete) = dlsym(handle,"OSAL_s32MessageDelete");
    *(void**)(&pu32GetSharedBaseAdress) = dlsym(handle,"pu32GetSharedBaseAdress");

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
    if(!OSAL_s32IOControl){
       fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;     
    }
 
    int b = OSAL_s32MessagePoolCreate(300000);
    printf("OSAL_s32MessagePoolCreate %d\n",b);

    OSAL_s32MessagePoolOpen();// should be 0 
    printf("pool current size: %d \n",OSAL_s32MessagePoolGetCurrentSize());
    int unk[2]= {0,0};
    int result = OSAL_s32MessageQueueOpen("mbx_1",4,unk);
    printf("Queue id %d %d %d\n",result,unk[0],unk[1]);
    int queueid = unk[0];
    int x[10] = {0};
    int y[10] = {0};
    int z[10] = {0};
    while(1==1){
    result = OSAL_s32MessageQueueWait(queueid,&x,8,&y,&z);
    printf("x %x %x %x %x\n",result, x[0] , x[1],x[2]);
    printf("y %x %x %x %x\n",result, y[0] , y[1],y[2]);
    printf("z %x %x %x %x\n",result, y[0] , y[1],y[2]);
    int msg_handle = x[1];
    
    result = OSAL_pu8MessageContentGet(*x,x[1],4);
    printf("content x %x %x %x %x\n",result, x[0] , x[1],x[2]);
    hexdump(result,40);    printf("\n");
    int k = *(int *)(result+16);
    printf("%x\n",k);
    result = pu32GetSharedBaseAdress();
    printf("result %x\n",result);

    hexdump((char *)(result),200);
}


    gets(buff);    

}    

