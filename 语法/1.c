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
    printf("%d", 0 ? 1 : 2);
    return 0;
}


int main() {
    func6();
    exit(0);
}
