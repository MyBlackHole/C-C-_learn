//
// Created by BlackHole on 2020/7/22.
//
#include "stdio.h"

int main() {
    int i[] = {1, 2};
    int *ii = i;

    int x = 0;
    int y = 2;
    printf("%d\n", x | y);
//    printf("%d", ii);

    // 与
    if (x | y) {
        printf("Black\n");
    }

    printf("%d\n", 1 & 3);
    printf("BlackHole");
}

