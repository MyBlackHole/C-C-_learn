#include <stdio.h>
#include <stdlib.h>

#define N 12

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

long combi(int n, int r) {
    int i;
    long p = 1;
    for (i = 1; i <= r; i++)
        p = p * (n - i + 1) / i;
    return p;
}

int main(int argc, char *argv[]) {
    int n, r, t;
    for (n = 0; n <= N; n++) {
        for (r = 0; r <= n; r++) {
            int i;
            if (r == 0) {
                for (i = 0; i <= (N - n); i++)
                    printf("  ");
            } else {
                printf(" ");
            }
            printf("%3ld", combi(n, r));
        }
        printf("\n");
    }
    return 0;
}
