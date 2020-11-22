//
// Created by BlackHole on 2020/7/16.
//
#include "stdio.h"

int add(int a, int b) {
    return a + b;
}

int main() {
    int c;
    c = add(1, 3);
    printf("%d", c);
    return 0;
}