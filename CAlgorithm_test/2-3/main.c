#include <stdio.h>
#include <stdlib.h>

/*  */

int main(int argc, char *argv[]) {
    double sum = 0;
    int i = 0;
    for (i;; i++) {
        double term = 1.0 / (i * 2 + 1);
        if (i % 2 == 0) sum += term;
        else sum -= term;
        if (term < 1e-6) break;
    }
    printf("%.6f\n", sum);
    return 0;
}
