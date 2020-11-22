//
// Created by Black Hole on 2020/11/14.
//

#include <stdio.h>
#include <stdlib.h>

int func();

int main() {
    func();
    exit(0);
}

/*
 * 逻辑 True ? 1: 2
 */
int func() {
    printf("%d", 0 ? 1 : 2);
    return 0;
}

