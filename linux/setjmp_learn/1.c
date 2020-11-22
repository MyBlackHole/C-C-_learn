//
// Created by Black Hole on 2020/11/22.
//

#include <stdio.h>
#include <setjmp.h>

/*
 * 1
 * setjmp: 保存当前运行堆栈
 * longjmp: 还原保存运行堆栈
 *
 *
 */

static jmp_buf jb;

int func1_2() {
    printf("我开始了\n");
    longjmp(jb, 9);
    printf("我开始没\n");
}

int func1() {
    printf("func1我先开始\n");
    int num;
    if ((num = setjmp(jb)) == 0)
        func1_2();
    printf("func1我结束了\n");
    printf("num: %d\n", num);
    return 0;
}

int
main() {
    func1();
    return 0;
}

