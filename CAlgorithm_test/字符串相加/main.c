//
// Created by BlackHole on 2020/8/3.
//

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char *addStrings(char *num1, char *num2) {
    size_t i = strlen(num1) - 1, j = strlen(num2) - 1, add = 0;
    char *ret = (char *) malloc(sizeof(char) * (i + j) + 3);
    int len = 0;
    while (i >= 0 || j >= 0 || add != 0) {
        int x = i >= 0 ? num1[i] - '0' : 0;
        int y = j >= 0 ? num2[j] - '0' : 0;
        int result = x + y + add;
        ret[len++] = '0' + result % 10;
        add = result / 10;
        i--, j--;
    }

    // 反转字符串
    for (int i = 0; 2 * i < len; i++) {
        int t = ret[i];
        ret[i] = ret[len - i - 1], ret[len - i - 1] = t;
    }
    ret[len++] = 0;
    return ret;
}

int main() {
    char c[] = {'1', '2', '3'};
//    printf("%ld", sizeof(int));
//    printf("%ld", sizeof(int));
//    printf("%d", c[1] - '0');
    printf("ok");
}
