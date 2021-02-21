#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    long long n, count = 0;
    scanf("%lld", &n);
    while (n > 1) {
        if (n % 2 == 1)
            n = n * 3 + 1;
        else n /= 2;
        count++;
    }
    printf("%d\n", count);
    return 0;
}
