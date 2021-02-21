//
// Created by black on 2020/10/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <wait.h>

int main() {
//    // 指向环境变量表
//    extern char **environ;
//    int i;
//    for (i = 0; environ[i] != NULL; i++)
//        printf("%s\n", environ[i]);

//    char s[10] = "PATH";
//    char s[10] = {'P', 'A', 'T', 'H'};
//    char *name = s;
//    printf("%s\n", getenv(name));

//    pid_t pid;
//    char *message;
//    int n;
//    pid = fork();
//    if (pid < 0) {
//        perror("fork failed");
//        exit(1);
//    }
//    if (pid == 0) {
//        message = "This is the child";
//        n = 6;
//    } else {
//        message = "This is the parent";
//        n = 3;
//    }
//    for (; n > 0; n--) {
//        printf("%s-%d-%d\n", message, pid, n);
//        sleep(1);
//    }
//
//    // 保证子进程运行完成
//    sleep(2);

//    execlp("ps", "-x", NULL);
//    perro("exec ps");
//    exit(1);


//    int ch;
//    while ((ch = getchar()) != EOF) {
//        putchar(toupper(ch));
//    }
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    if (pid == 0) {
        int i;
        for (i = 3; i > 0; i--) {
            printf("This is the child\n");
            sleep(1);
        }
        exit(3);
    } else {
        int stat_val;
        waitpid(pid, &stat_val, 0);
        if (WIFEXITED(stat_val))
            printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        else if (WIFSIGNALED(stat_val))
            printf("Child terminated abnormally, signal %d\n", WTERMSIG(stat_val));
    }
    return 0;
}

