#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 1010
int a[maxn];

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    int n, k, first = 1;
    memset(a, 0, sizeof(a));
    scanf("%d%d", &n, &k);
    int i;
    for (i = 1; i <= k; i++) {
        int j;
        for (j = 1; j <= n; j++)
            if (j % i == 0)
                a[j] = !a[j];
    }
    for (i = 1; i <= n; i++)
        if (a[i]) {
            if (first)
                first = 0;
            else
                printf(" ");
            printf("%d", i);
        }
    printf("\n");
    return 0;
}
