#include <stdio.h>
#include <stdlib.h>

#define INF 1001

/* 求最大值与最小值和均值 */

int main(int argc, char *argv[]) {

    freopen("../2-5/input.txt", "r", stdin);
    freopen("../2-5/output.txt", "w", stdout);

    int x, n = 0, min, max, s = 0;
    max = -INF;
    min = +INF;
    while (scanf("%d", &x) == 1) {
        s += x;
        if (x < min) min = x;
        if (x > max) max = x;
        n++;
    }
    printf("%d %d %.3f\n", min, max, (double) s / n);
    return 0;
}
