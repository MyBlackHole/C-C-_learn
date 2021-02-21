//
// Created by BlackHole on 2020/7/22.
//
#include <stdio.h>
#include <string.h>

int main() {
////    strcpy
//    char buf[10];
//    strcpy(buf, "hello");
//    printf("%sok", buf);

//    strncpy
    char buf[10];
    strncpy(buf, "hello world", sizeof(buf));
    printf("%sok", buf);
}
