#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int f(int n) {
    return n == 0 ? 1 : f(n - 1) * n;
}

int main(int argc, char *argv[]) {
    printf("%d\n", f(10));
    return 0;
}
