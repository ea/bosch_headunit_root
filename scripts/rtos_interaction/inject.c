#include <stdio.h>
#include "util.h"
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
	printf("OSALUTIL_s32TraceWrite: %s\n",s);
}

int vWriteToErrMem(int x ,char *s,int y, char *z){
		printf("vWriteToErrMem: %s\n",s);

}