//
// Created by Black Hole on 2020/11/12.
//
#include <stdio.h>

void func();


int main() {
//    void (*p)() = &func;
    void (*p)() = func;
    p();
    return 0;
}

void func() {
    printf("func\n");
}
