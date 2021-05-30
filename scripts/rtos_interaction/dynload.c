
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <dlfcn.h>
#include "dynload.h"
#include <unistd.h>

int dynload(){

handle = (struct link_map*)  dlopen("/opt/bosch/processes/libosal_linux_so.so", RTLD_LAZY);
    if (!handle) {
    fprintf(stderr, "Error: %s\n", dlerror());
    return -1;
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
*(void**)(&vDebugFacility) = dlsym(handle,"vDebugFacility");
*(void**)(&vPrintSharedMemoryTable) = dlsym(handle,"vPrintSharedMemoryTable");
*(void**)(&vPrintMsgInfo) = dlsym(handle,"vPrintMsgInfo");

pOsalData = dlsym(handle,"pOsalData");

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
return 0;

}

void cleanup(){
    //add whatever else cleanup is neccessary

    _exit(0); //till i figure why it crashes in osal destructors...


    puts("\npress any key to continue\n");
    getchar();    

    dlclose(handle);

}