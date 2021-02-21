#include <stdio.h>
#include <stdlib.h>

#define maxn 105
int a[maxn];

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    int x, n = 0;
    while (scanf("%d", &x) == 1)
        a[n++] = x;
    int i;
    for (i = n - 1; i >= 1; i--)
        printf("%d ", a[i]);
    printf("%d\n", a[0]);
    return 0;
}
