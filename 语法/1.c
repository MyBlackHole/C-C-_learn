//
// Created by Black Hole on 2020/11/14.
//

#include <stdio.h>
#include <stdlib.h>

/*
 * 1
 * 逻辑 True ? 1: 2
 *
 * 2
 * |=
 *
 * 3
 * &=
 *
 * 4
 * ~
 *
 * 5
 * 十进制、八进制、十六进制
 *
 */
struct test1
{
    /* data */
    int a : 2;
    int b;
} tt;


int func9() {
    struct test1 ttt;
    ttt.b = 9;
    printf("%p\n", &tt);
    printf("%p\n", &tt.b);
    unsigned long int count = (unsigned long int)&tt.b - (unsigned long int)&tt;
    printf("%lu\n", count);
    printf("%p\n", &ttt);
    printf("%p\n", &ttt.b);
    unsigned long int a = (unsigned long int)&ttt;
    printf("%lu\n", a);

    ttt.a = 1;

    // int 
    printf("%d\n", *(int *)((unsigned long int)&ttt + count));
    printf("%d\n", ttt.a);
    return 0;
}


int func8() {
    printf("%p\n", &tt);
    printf("%p\n", &tt);
    printf("%lu\n", sizeof(tt));
    return 0;
}

struct test
{
    /* data */
    int a : 2;
};

int func7() {
    struct test t;
    printf("%d\n", t.a);
    return 0;
}



int func6() {
    int i = 1000;
    printf("八进制:%d\n", (i));
    return 0;
}


int func5() {
    int i = 1000;
    printf("八进制:%o\n", i);
    printf("十进制:%d\n", i);
    printf("十六进制:%X\n", ~0);
    return 0;
}

int func4() {
    int i = 0;
    printf("%d", ~i);
    return 0;
}

int func3() {
    int i = 0;
    i &= 1 << 2;
    printf("%d", i);
    return 0;
}

int func2() {
    int i = 0;
    i |= 1 << 2;
    printf("%d", i);
    return 0;
}


int func1() {
    printf("%d\n", 1 ? 1 : 2);
    return 0;
}


int main() {
    func9();
    exit(0);
}
