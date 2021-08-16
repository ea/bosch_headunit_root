#include <stdio.h>
#include "util.h"
#define _GNU_SOURCE
#include <dlfcn.h>
void TraceString(char *s){
        printf("TRACE: %s\n",s);
}

int bFilterMsg(int *p){return 1;}

int vPrintMessage(int fd, char*s){
	printf("TRACE: %s\n",s);
}

int TraceIOString(char *s){
	printf("TRACE %s\n",s);
}

int LLD_vTrace(int x, int y, char *s, int z){

	printf("LLD_vTrace: %s\n",s+13);
}

int OSALUTIL_s32TraceWrite(int x , int y, int z, char *s, int j){
	printf("OSALUTIL_s32TraceWrite: %s %x %x %x %x\n",s+1,x,y,z,j);
}

int vWriteToErrMem(int x ,char *s,int y, char *z){
		printf("vWriteToErrMem: %s\n",s);

}


/*
int (*OSAL_s32IOControlOrig)(int fd, int code,int *buff) = 0;

int OSAL_s32IOControl(int fd,int code, int *contents){
	if(OSAL_s32IOControlOrig == 0){
	void *handle = (struct link_map*)  dlopen("/opt/bosch/processes/libosal_linux_so.so", RTLD_LAZY);
	*(void**)(&OSAL_s32IOControlOrig) = dlsym(handle,"OSAL_s32IOControl");	
		
	}

	if(code == 3){
		if(contents[1] == 0x8b01 ||contents[1] == 0x8b00 ){
			contents[0] = 0x01;
			return 0;
		}
		if (contents[1] == 0x2713  ){
			printf("enavi_TclTrace");
			contents[0] = 0x01;
			return 0;

		}
	}
	return OSAL_s32IOControlOrig(fd,code,contents);




}*/


