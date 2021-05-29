int OSAL_ProcessWhoAmI(){

        printf("HEEEEEELYEAAAAAAAAAA\n");

}

void vTraceCcaMsg(int *param_1,int param_2,int param_3,int param_4){
        printf("vTraceCcaMsgvTraceCcaMsgvTraceCcaMsgvTraceCcaMsgvTraceCcaMsgvTraceCcaMsgvTraceCcaMsg\n");


}

int  etg_bIsTraceActiveShort(int a){
    printf("IS TRACE ACTIVE %d",a);
    return 1;
}
void etg_vTraceBinary(int a){
    printf("V_TRACE_BINARY %d",a);
    
}

    //OSAL_vPrintMessageList(1);
    //vOsalTraceOutRegistry("/dev/registry");

    printf("OsalData in SHM: %p\n", (void *)*pOsalData);
    uint8_t *enable_logging = (uint8_t*)(*pOsalData + 0x2cd49);
    printf("wth %x\n", *(char *)(*pOsalData + 0x2cd49 ));
    *enable_logging = 1;
    printf("wth %x\n", *(char *)(*pOsalData + 0x2cd49 ));
    vPrintSharedMemoryTable();


        //bReadEntry(this,0xdfe,0x18,(uchar *)(this + 5));
  

  /* char * word1 = (unsigned short int)strtol(argv[1],0,16);
   unsigned short int word2 = (unsigned short int)strtol(argv[2],0,16);
    readbuff[0] = word1+i;
    readbuff[1] = word2;*/