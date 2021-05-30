#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dynload.h"
#include "util.h"




int main(int argc, char **argv){


   char readbuff[2000];
   memset(readbuff,'\x00',2000);


    if(argc != 3){
    printf("Usage example: ./%s /dev/root/proc/mounts 1|4\n",argv[0]);
        return 1;
    }   
    if(dynload() != 0) return 1;


    
    int fp = OSAL_IOOpen(argv[1],atoi(argv[2]));
    if(fp == 0xffffffff) {
    	printf("No such device: %s\n",argv[1]);
    	exit(0);
    }

    int r = 0 ; 
    while((r = OSAL_s32IORead(fp,readbuff,2000)) > 0)  {
        dump(readbuff,r);
    }
    cleanup();
}    

