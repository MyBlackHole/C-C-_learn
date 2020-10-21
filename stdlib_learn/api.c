//
// Created by black on 2020/10/21.
//

#include <stdio.h>
#include <stdlib.h>

int main() {
    char s[10] = {'P', 'A', 'T', 'H'};
    char *name = s;
    printf("%s\n", getenv(name));
    return 0;
}