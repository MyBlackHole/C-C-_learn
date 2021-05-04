//
// Created by BlackHole on 2020/7/16.
//
#include "stdio.h"

typedef unsigned long UINTPTR;
typedef unsigned int UINT32;

int add(int a, int b) {
    return a + b;
}

UINTPTR LOS_Align(UINTPTR addr, UINT32 boundary) {
    if ((addr + boundary - 1) > addr) {
        return (addr + boundary - 1) & ~((UINTPTR) (boundary - 1));
    } else {
        return addr & ~((UINTPTR) (boundary - 1));
    }
}

int function(void) {
    static int i, state = 0;
    switch (state) {
        case 0: goto LABEL0;
        case 1: goto LABEL1;
    }
    LABEL0: /* start of function */
    for (i = 0; i < 10; i++) {
        state = 1; /* so we will come back to LABEL1 */
        return i;
        LABEL1:; /* resume control straight after the return */
    }
}



int main(int argc, char *argv[]) {
    printf("%d", function());
    printf("%d", function());
    printf("%d", function());
    printf("%d", function());

    // int t[2] = {0};
    // printf("%d-%d", t[0], t[1]);
    // if ( 1 ){
    //     printf("%d", 0);
    // }
//    printf("%d", argc);
//    for (int i = 0; i < argc; i++) {
//        printf("%s", argv[i]);
//    }
//    int input = 8;
//    int c;
//    c = LOS_Align(7, 3);
//    printf("%d", c);
//    return 0;
}
