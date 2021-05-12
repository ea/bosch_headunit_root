
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
    int (*OSAL_pu8MessageContentGet)(unsigned int *p1,unsigned int *p2,unsigned int code);

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
    int fp = OSAL_IOOpen(argv[1],4);
    if(fp == 0xffffffff) {
    	printf("No such device: %s\n",argv[1]);
    	exit(0);
    }
    //bReadEntry(this,0xdfe,0x18,(uchar *)(this + 5));
  

  /* char * word1 = (unsigned short int)strtol(argv[1],0,16);
   unsigned short int word2 = (unsigned short int)strtol(argv[2],0,16);
    readbuff[0] = word1+i;
    readbuff[1] = word2;*/

    
   // get sub path
    ulong b1[2];
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    int b = 0 ; 
    printf("descendants:\n");
    while ( b =  OSAL_s32IOControl(fp,5,&readbuff) != -1){
    printf("%s\n",&readbuff[2]);
    }
    // b = OSAL_s32IOControl(fp,5,&readbuff);
    //printf("%d %s %d %d\n",b,&readbuff[2],b1[0],b1[1]);
    
    
    // get key name
    printf("keys: \n");
    readbuff[0] = fp;
    readbuff[1] = 0;
    readbuff[2] = 0;
    readbuff[3] = 0;
    while( b = OSAL_s32IOControl(fp,2,&readbuff) != -1){
    printf("%s\n",&readbuff[2]);
    }
    
    //query key value
    char *p = argv[2];
    readbuff[0] = p;
    readbuff[1] = 4;
    readbuff[2] = 0;
    readbuff[3] = &b1;
     b = OSAL_s32IOControl(fp,1,&readbuff);
    printf("%d %d %d %d\n",b,readbuff[2],b1[0],b1[1]);

    b = OSAL_s32MessagePoolCreate(300000);
    printf("OSAL_s32MessagePoolCreate %d\n",b);

    OSAL_s32MessagePoolOpen();// should be 0 
    printf("pool current size: %d \n",OSAL_s32MessagePoolGetCurrentSize());

    gets(buff);    

    OSAL_s32IOClose(fp);
}    

