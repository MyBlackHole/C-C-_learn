#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int pan(int a, int b, int c) {
    if (a * 10 + b > 33) {
        return 9;
    } else {
        int i;
        int d[9];
        int j = 1;
        for (i = 0; i < 3; i++) {
            d[2 + 3 * i] = c * (i + 1) % 10;
            d[1 + 3 * i] = (b * (i + 1) + c * (i + 1) / 10) % 10;
            d[0 + 3 * i] = (a * (i + 1) + (b * (i + 1) + c * (i + 1) / 10) / 10) % 10;
            j *= d[0 + 3 * i] * d[1 + 3 * i] * d[2 + 3 * i];
        }
        if (j == 362880)
            printf("%d%d%d %d%d%d %d%d%d\n", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
        return 0;
    }
}

int main(int argc, char *argv[]) {
    int s[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
    for (i = 0; i <= 3; i++) {
        int j;
        for (j = 0; j < 9; j++) {
            if (i == j)
                continue;
            int k;
            for (k = 0; k < 9; k++) {
                if (i == k || k == j)
                    continue;
                j = pan(s[i], s[j], s[k]) + j;
            }
        }
    }
    return 0;
}
