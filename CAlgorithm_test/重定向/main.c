#include <stdio.h>
#include <stdlib.h>

#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("../重定向/data.in", "r", stdin);
    freopen("../重定向/data.out", "w", stdout);
#endif
    int dig, max, min, count = 1;
    double aver;
    scanf("%d", &dig);
    max = min = dig;
    int sum = dig;
    while (scanf("%d", &dig) != EOF) {
        if (dig > max) max = dig;
        else if (dig < min) min = dig;
        count++;
        sum += dig;
    }
    aver = (double) sum / (double) count;
    printf("%d %d %lf", min, max, aver);
    getchar();
    getchar();
    return 0;
}
