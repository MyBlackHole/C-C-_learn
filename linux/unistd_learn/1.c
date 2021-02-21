//
// Created by Black Hole on 2020/11/8.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

/*
 * 1
 * truncate: 文件截断
 * link: 文件连接
 *
 * 2
 * sysconf: 获取系统变量
 * pathconf: 获取path里的变量
 *
 * 3
 * sbrk: 内存分配
 * brk: 内存分配
 *
 * 4
 * fork: 创建子进程
 *
 * 5
 * alarm: 定时器 产生SIGALRM
 * pause: 等待到收到信号为止
 */

void
func5_1() {
    printf("func5_1");
}

void
func5() {
    if (signal(SIGALRM, func5_1) == SIG_ERR)
        printf("signal - %s\n", strerror(errno));
    alarm(1);
    pause();
    exit(0);
}

void
func4() {
    printf("0\n");
    pid_t pid;
    if ((pid = fork()) < 0) {
        printf("error: %s", strerror(errno));
    } else if (pid == 0) {
        printf("创建子进程成功\n");
    } else {
        printf("我是父进程成功\n");
    }
    exit(0);
}

void
func3() {
    int i;
    int *former;
    int *previous;
    int *end;
    previous = sbrk(0);
    former = previous;

    for (i = 0; i < 10; i++) {
        brk(previous + 1);
        *previous = i;
        previous = sbrk(0);
//        end = sbrk(0);
//        previous++;
    }
    previous = former;
    printf("%p\n", sbrk(0));
    printf("%p\n", sbrk(0));
    printf("%p\n", sbrk(0));

    while (sbrk(0) != previous) {
//    while (previous != end) {
        printf("%d\n", *previous);
        printf("%p\n", sbrk(0));
        previous++;
        exit(0);
    }
    brk(former);
}

void
func2() {
    static long int posix_version = 0;
    // sysconf: 获取系统变量
    posix_version = sysconf(_SC_VERSION);
    printf("%ld\n", posix_version);
    long int pathmax;
    // pathconf: 获取path里的变量
    pathmax = pathconf("/", _PC_PATH_MAX);
    printf("%ld\n", pathmax);
    exit(0);
}

void
func1() {
    int status;
    // truncate: 文件截断
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

    // link: 文件连接
    link("truncate.txt", "truncate1.txt");

    exit(0);
}


int
main() {
    func5();
}

