//
// Created by BlackHole on 2020/7/15.
//
#include <stdio.h>

int main() {
    int input = 8;
    int result = 0;
    __asm__ __volatile__ ("movl %1,%0" : "=r" (result) : "r" (input));
    printf("%d\n", result);
    return 0;
}

