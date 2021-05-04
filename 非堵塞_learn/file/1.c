#include <unistd.h> // UNIX C标准库
#include <fcntl.h> // 是unix标准中通用的头文件，其中包含的相关函数有 open，fcntl，shutdown，unlink，fclose等！
#include <errno.h> // 定义错误码
#include <string.h> // 字符串处理
#include <stdlib.h> // 定义输入输出函数
 
#define MSG_TRY "try again\n"
#define MSG_TIMEOUT "timeout\n"
 
int main(void) {
    char buf[10];
    int fd, n, i;
    // 只读方式打开|以不可阻断的方式打开文件，也就是无论有无数据读取或等待，都会立即返回进程之中。
    fd = open("/home/black/Documents/Code/C_CJJ/C-C++_learn/非堵塞_learn/file/test.txt", O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        exit(1);
    }
    for(i=0; i<5; i++) {
        n = read(fd, buf, 10);
        if (n>=0) {
            break;
        }
        // errno 记录系统最后一次错误
        // EAGAIN 如果你连续做read操作而没有数据可读，此时程序不会阻塞起来等待数据准备就绪返回，read函数会返回一个错误EAGAIN，提示你的应用程序现在没有数据可读请稍后再试。
        if (errno != EAGAIN) {
            exit(1);
        }
        sleep(1);
        write(STDOUT_FILENO, MSG_TRY, strlen(MSG_TRY));
    }
    if(i==5) {
        write(STDOUT_FILENO, MSG_TIMEOUT, strlen(MSG_TIMEOUT));
    }
    else {
        write(STDOUT_FILENO, buf, n);
    }
    close(fd);
    return 0;
}