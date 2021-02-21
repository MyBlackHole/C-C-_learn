#include <stdio.h>

#define LOCAL

/* 替换输入输出流反转 */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("../1-7/data.in", "r", stdin);
    freopen("../1-7/data.out", "w", stdout);
#endif
    int n, m;
    scanf("%d", &n);
    m = (n % 10) * 100 + (n / 10 % 10) * 10 + (n / 100);
    printf("%03d\n", m);
    return 0;
}
