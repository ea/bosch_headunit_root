#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "util.h"
#include "dynload.h"

// this test program demonstrates how to use functions from OSAL to
// write KDS which is some sort of a configuration store



int main(int argc, char **argv){

	union {
		char rawdata[1024];
		struct {
			unsigned short code;
			unsigned short len;
			unsigned short unknown;
			char data[1012];
		} ioread;
	} buffer;

	if (argc < 4) {
		printf("usage: %s <dev> <code> <data>\n", argv[0]);
		return EXIT_FAILURE;
	}

    if(dynload() != 0) return 1;


	int fp = OSAL_IOOpen(argv[1],4);
	if (fp == 0xffffffff) {
	    printf("No such device: %s\n",argv[1]);
		return EXIT_FAILURE;
	}

	memset(buffer.rawdata, 0, sizeof(buffer));
	OSAL_s32IOControl(fp,2,(int*)1);
	buffer.ioread.code = (unsigned short)strtol(argv[2],0,16);;
	buffer.ioread.len = strlen(argv[3]);
	strcpy(buffer.ioread.data,argv[3]);
	OSAL_s32IOWrite(fp,buffer.rawdata,0xf6); 
	OSAL_s32IOControl(fp,12,(int*)1); // flush write


	OSAL_s32IOClose(fp);
	cleanup();
	return EXIT_SUCCESS;
}
