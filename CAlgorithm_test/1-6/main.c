#include <stdio.h>
#include <stdlib.h>

#define LOCAL

/* 替换输入输出流反串 */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("../1-6/data.in", "r", stdin);
    freopen("../1-6/data.out", "w", stdout);
#endif
    int n;
    scanf("%d", &n);
    printf("%d%d%d", n % 10, n / 10 % 10, n / 100);
    return 0;
}
