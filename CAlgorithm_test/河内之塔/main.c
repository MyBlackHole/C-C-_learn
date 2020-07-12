#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void hanoi(int n, int A, int B, int C) {
    if (n == 1) {
        printf("Move sheet%d from %c to %c\n", n, A, C);
    } else {
        hanoi(n - 1, A, C, B);
        printf("Move sheet%d from %c to %d\n", n, A, C);
        hanoi(n - 1, B, A, C);
    }
}

int main(int argc, char *argv[]) {
    int n;
    printf(": ");
    scanf("%d", &n);
    hanoi(n, 'A', 'B', 'C');
    return 0;
}
