
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <dlfcn.h>
#include "osal_types.h"
#include <stdio.h>

void *handle;
int (*OSAL_IOOpen)(const char*,int param);
void (*OSAL_s32IOClose)(int fp);
int (*OSAL_s32IORead)(int fp,char *buff,int code);
int (*OSAL_s32IOControl)(int fp,unsigned int f,char *buff);
int (*OSAL_s32MessagePoolCreate)(ulong size);
int (*OSAL_s32MessagePoolOpen)();
int (*OSAL_s32MessagePoolGetCurrentSize)();
void (*OSAL_vPrintMessageList)( int fd);
void (*vOsalTraceOutRegistry)(char *path);
MessageStruct * (*OSAL_pu8MessageContentGet)(unsigned int *p1,unsigned int *p2,unsigned int code);
int (*OSAL_s32MessageQueueOpen)(char *queuename,int x, int *unknown);
int (*OSAL_s32MessageQueueStatus)(int queueid,int *x, int *y);
int (*OSALUTIL_prOpenDir)(const char*);
char* (*OSALUTIL_prReadDir)(int );
int (*OSAL_s32MessageQueueWait)(int queueid,int **x, int i,int **y,int **z);
int (*s32ReadCompleteMsg)(int msg,int x);
int (*u32GetActualMsg)(int msg);
int (*OSAL_s32MessageDelete)(int msg_type,int msg_handle);
void * (*pu32GetSharedBaseAdress)();
void * (*vDebugFacility)();
int *pOsalData;
void * (*vPrintSharedMemoryTable)();
void (*vPrintMsgInfo)(char *,uint8_t flag);

int dynload();
void cleanup();