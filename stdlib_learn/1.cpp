//
// Created by BlackHole on 2020/7/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int val;
    char str[20];

    strcpy(str, "98993489");
    val = atoi(str);
    printf("字符串值 = %s, 整型值 = %d\n", str, val);

    strcpy(str, "runoob.com");
    val = atoi(str);
    printf("字符串值 = %s, 整型值 = %d\n", str, val);

    return (0);
}
