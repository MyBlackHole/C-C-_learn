//
// Created by black on 2020/10/21.
//

#include <stdio.h>
#include <stdlib.h>

int main() {
    char s[10] = {'P', 'A', 'T', 'H'};
    char *name = s;

//    // 获取环境变量
//    printf("%s\n", getenv(name));

    // 修改环境变量
    printf("%s\n", getenv(name));
    setenv("PATH", "hello", 1);
    printf("%s\n", getenv(name));

    return 0;
}