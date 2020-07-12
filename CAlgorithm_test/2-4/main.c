#include <stdio.h>
#include <time.h>

/* 程序运行时间 */

int main(int argc, char *argv[]) {
    const int MOD = 1000000;
    int n, i, s = 0;
    scanf("%d", &n);
    printf("ok");
    printf("\n");
    printf("%d", n);
    printf("\n");
    for (i = 1; i <= n; i++) {
        int factorial = 1;
        int j = 1;
        for (j; j <= i; j++)
            factorial = (factorial * j % MOD);
        s = (factorial + s) % MOD;
        printf("%d\n", s);
    }
    printf("%d\n", s);
    printf("Time used = %.2f\n", (double) clock() / CLOCKS_PER_SEC);
    return 0;
}
