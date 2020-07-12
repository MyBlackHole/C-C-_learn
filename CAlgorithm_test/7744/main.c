#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    int x = 1;
    for (x;; x++) {
        int n = x * x;
        if (n < 1000) continue;
        printf("            %d\n", n);
        if (n > 9999) break;
        int hi = n / 100;
        int lo = n % 100;
        if (hi / 10 == hi % 10 && lo / 10 == lo % 10)
            printf("%d\n", n);
    }
    return 0;
}
