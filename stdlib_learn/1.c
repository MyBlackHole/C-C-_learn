//
// Created by BlackHole on 2020/7/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void msg();

int main(int argc, char **argv) {
//    int val;
//    char str[20];
//
//    strcpy(str, "98993489");
////  atoi 字符串->整型值
//    val = atoi(str);
//    printf("字符串值 = %s, 整型值 = %d\n", str, val);
//
//    strcpy(str, "runoob.com");
//    val = atoi(str);
//    printf("字符串值 = %s, 整型值 = %d\n", str, val);
    msg();
    return (0);
}

// msg

typedef struct {
    int number;
    char *msg;
} unit_t;

void msg() {
    unit_t *p = malloc(sizeof(unit_t));
    if (p == NULL) {
        printf("out of memory\n");
        exit(1);
    }
    p->number = 3;
    p->msg = malloc(50);
    strcpy(p->msg, "Hello world!");
    printf("number: %d\nmsg: %s\n", p->number, p->msg);
    free(p->msg);
    free(p);
    p = NULL;
    printf("%ld, int:%ld", sizeof(unit_t), sizeof(NULL));
    return;
}