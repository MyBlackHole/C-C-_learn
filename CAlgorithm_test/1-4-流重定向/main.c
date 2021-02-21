#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>

#define LOCAL

/* 文件的读取与写入 */

int main(int argc, char *argv[]) {
    char *dir = getcwd(NULL, 0); // 确定工作目录
    printf("%s\n", dir);
#ifdef LOCAL
    freopen("../1-4-流重定向/data.in", "r", stdin);
    freopen("../1-4-流重定向/data.out", "w", stdout);
#endif
//    printf("%ls", fd);
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d %d\n", a, b);
    printf("%d\n", a + b);
    return 0;
}
