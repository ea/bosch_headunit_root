#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dynload.h"
#include "util.h"

// ex: get all message queues
//LD_PRELOAD=./inject.so ./trace_hooking_demo 4 `echo -e "AA\x1f"` 
//help
//LD_PRELOAD=./inject.so ./trace_hooking_demo 4 `echo -e "AA\xf0"` #for help
//LD_PRELOAD=./inject.so ./trace_hooking_demo 4 `echo -e "AA\x12\x12"`#trace open files
int main(int argc, char **argv){

	if(argc < 2){
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
    	case 4:
    			vSysCallbackHandler(argv[2]);
    			break;
    }



    cleanup();


}

