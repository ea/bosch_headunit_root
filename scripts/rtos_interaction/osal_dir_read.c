#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "dynload.h"
#include "util.h"




int main(int argc, char **argv){

	if(argc != 2){
    printf("Usage example: ./%s /dev/ffs\n",argv[0]);
    	return 1;
	}	
    if(dynload() != 0) return 1;

    int r = OSALUTIL_prOpenDir(argv[1]);
    printf("%x\n\n\n",r);
    char *dr; 
    while(dr = OSALUTIL_prReadDir(r)){
	    printf("%s\n",dr);

    	
	}

	cleanup();

}
