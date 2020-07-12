#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LOCAL

/* 替换输入输出流 */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("../1-4-流重定向/data.in", "r", stdin);
    freopen("../1-4-流重定向/data.out", "w", stdout);
#endif
    const double pi = acos(-1.0);
    double r, h, s1, s2, s;
    scanf("%lf%lf", &r, &h);
    s1 = pi * r * r;
    s2 = 2 * pi * r * h;
    s = s1 * 2.0 + s2;
    printf("Area = %.3f\n", s);
    return 0;
}
