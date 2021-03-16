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


int main(int argc, char *argv[]) {
    int t[2] = {0};
    printf("%d-%d", t[0], t[1]);
    if ( 1 ){
        printf("%d", 0);
    }
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
