#include <stdio.h>
#include <stdlib.h>

#define N 20

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
//fn = fn-1 + fn-2     if n > 1
//fn = n		   		if n = 1, 0 


int main(int argc, char *argv[]) {
    int Fib[N] = {0};
    int i;

    Fib[0] = 0;
    Fib[1] = 1;

    for (i = 2; i < N; i++)
        Fib[i] = Fib[i - 1] + Fib[i - 2];

    for (i = 0; i < N; i++)
        printf("%d\n", Fib[i]);
    printf("\n");
    return 0;
}
