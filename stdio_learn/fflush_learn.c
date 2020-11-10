//
// Created by Black Hole on 2020/11/2.
//
#include <stdio.h>

int main() {
    char c;
//    scanf("%c",&c);
    scanf_s("%c",&c);
    printf("%c", c);

    // 清理 stdin 流
    fflush(stdin);
//    scanf("%c",&c);
    scanf_s("%c",&c);
    printf("%c", c);
//    scanf_s("%c",&c);
    return 0;
}
