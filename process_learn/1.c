//
// Created by black on 2020/10/21.
//

#include <stdio.h>
#include <stdlib.h>

int main() {
    // 指向环境变量表
    extern char **environ;
    int i;
    for (i = 0; environ[i] != NULL; i++)
        printf("%s\n", environ[i]);

//    char s[10] = "PATH";
    char s[10] = {'P', 'A', 'T', 'H'};
    char *name = s;
    printf("%s\n", getenv(name));
    return 0;
}

