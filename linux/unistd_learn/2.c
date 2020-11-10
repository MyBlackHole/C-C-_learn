//
// Created by Black Hole on 2020/11/9.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    static long int posix_version = 0;
    posix_version = sysconf(_SC_VERSION);
    printf("%ld\n", posix_version);
    long int pathmax;
    pathmax = pathconf("/", _PC_PATH_MAX);
    printf("%ld\n", pathmax);
    exit(0);
}

