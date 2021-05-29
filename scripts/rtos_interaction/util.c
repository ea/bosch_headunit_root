#include "util.h"

void dump(char *p, int size) {
    for (int i = 0; i < size; i+=32) {
        if(i && !(i % 32)) printf("\n");
        for (int j = 0; j < 32; j++) {
            if (i + j >= size) {
                for (int k = j; k < 32; k++) {
                    if (k == 16) printf(" ");
                    printf("  ");
                }
                break;
            }
            if (j == 16) printf(" ");
            printf("%02x", p[i+j]);
        }
        printf("  ");
        for (int j = 0; j < 32; j++) {
            if (i+j >= size) break;
            if (p[i+j] >= 0x21 && p[i+j] <= 0x7e) {
                printf("%c", p[i+j]);
            } else {
                printf(".");
            }
        }
    }
    printf("\n");
}


void printstrings(char *p, int size){

    int total_printed = 0;
    int printed = 0;
    while(total_printed < size){
        printed = printf("%s",p+total_printed);
        if(printed == 0){printed = 1;}else{printf("\n");}
        total_printed += printed;
    }
}
