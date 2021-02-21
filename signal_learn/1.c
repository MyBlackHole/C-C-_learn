//
// Created by Black Hole on 2020/11/22.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
 * 1
 * sigprocmask: 设置或者查询信号启用状态
 *
 * 2
 * raise: 向自身进程发送信号
 *
 * 3
 * kill: 发送信号给进程或进程组
 *
 * 4
 * sigaction: 检查或修改（或检查并修改）
 *
 */


void
func4_1(int signo) {
    printf("func4_1");
}


void
func4() {
    struct sigaction act;
    struct sigaction oldact;
    act.sa_handler = func4_1;
    act.sa_flags = SA_NODEFER;//| SA_RESETHAND;
    sigaction(SIGALRM, &act, &oldact);
    printf("this is the main function\n");
    pid_t pid = getpid();
    printf("%d\n", pid);
    alarm(1);
    pause();
//    kill(pid, SIGALRM);
}

void
func3_1(int signo) {
    printf("func3_1");
}

void
func3() {
    pid_t pid;
    if (signal(SIGCLD, func3_1) == SIG_ERR)
        perror(strerror(errno));
    kill(getpid(), SIGCLD);
    exit(0);
}

void
func2_1() {
    printf("func2_1");
}

void
func2() {
    if (signal(SIGCLD, func2_1) == SIG_ERR)
        perror(strerror(errno));
    raise(SIGCLD);
    exit(0);
}

void
func1() {
    sigset_t set;
    sigprocmask(0, NULL, &set);
    for (int i = 0; i < _SIGSET_NWORDS; i++)
        printf("__val[%d] - %ld\n", i, set.__val[i]);
    exit(0);
}

int
main() {
    func4();
    exit(0);
}