#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dynload.h"
#include "util.h"

static void TraceString(char *s){
        printf("TRACE: %s\n",s);


}
static int bFilterMsg(int *p){return 1;}

static int vPrintMessage(int fd, char*s){
	printf("TRACE: %s\n",s);
}

static int TraceIOString(char *s){
	printf("TRACE %s\n",s);
}

static int LLD_vTrace(int x, int y, char *s, char *z){

	printf("LLD_vTrace: %s\n",s);
}

static int OSALUTIL_s32TraceWrite(int x , int y, int z, char *s, int j){
	printf("OSALUTIL_s32TraceWrite: %s\n",s);
}

static int vWriteToErrMem(int x ,char *s,int y, char *z){
		printf("vWriteToErrMem: %s\n",s);

}


int main(int argc, char **argv){

	if(argc != 2){
    printf("Usage example: LD_PRELOAD=./inject.so ./trace_hooking_demo 1|2|3|...\n");
    	return 1;
	}	
    if(dynload() != 0) return 1;

	printf("OsalData in SHM: %p\n", (void *)*pOsalData);
    uint8_t *enable_logging = (uint8_t*)(*pOsalData + 0x2cd49);
    printf("Tracing? %x\n", *(char *)(*pOsalData + 0x2cd49 ));
    *enable_logging = 1;
    printf("Tracing? %x\n", *(char *)(*pOsalData + 0x2cd49 ));


    int choice  = atoi(argv[1]);
    switch(choice){
    	case 1:
    		    vPrintSharedMemoryTable();
    		    break;
	    case 2:
	    		OSAL_vPrintMessageList(1);
    			break;
		case 3:
    			vOsalTraceOutRegistry("/dev/registry");
    			break;
    }



    cleanup();


}

