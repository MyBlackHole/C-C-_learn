//
// Created by Black Hole on 2020/11/8.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main() {
    int status;
    if ((status = truncate("truncate.txt", 10)) < 0) {
//        error("truncate.txt 截取失败");
        printf("%s", strerror(errno));
    }
    int fd;
    if ((fd = open("truncate.txt", O_RDWR)) < 0) {
        printf("%s", strerror(errno));
    }
//    if ((ftruncate(fd, 0)) < 0)
//        printf("%s", strerror(errno));
    printf("%d", status);

    link("truncate.txt", "truncate1.txt");

    exit(0);
}

