#include <stdio.h>
#include <stdlib.h>

#define maxn 1010

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    FILE *fin, *fout;
    fin = fopen("data.in", "r");
    fout = fopen("data.out", "w");
    int n, a[maxn], b[maxn];
    int kase = 0;
    while (fscanf(fin, "%d", &n) == 1 && n) {
        fprintf(fout, "Game %d:\n", ++kase);
        int i;
        for (i = 0; i < n; i++)
            fscanf(fin, "%d", &a[i]);
        for (;;) {
            int A = 0;
            int B = 0;
            int i;
            for (i = 0; i < n; i++) {
                fscanf(fin, "&d", &b[i]);
                if (a[i] == b[i])
                    A++;
            }
            if (b[0] == 0)
                break;
            int d;
            for (d = 1; d <= 9; d++) {
                int c1 = 0;
                int c2 = 0;
                int i;
                for (i = 0; i < n; i++) {
                    if (a[i] == d)
                        c1++;
                    if (b[i] == d)
                        c2++;
                }
                if (c1 < c2)
                    B += c1;
                else
                    B += c2;
            }
            fprintf(fout, "    (%d,%d)\n", A, B - A);
        }
    }
    fclose(fin);
    fclose(fout);
    return 0;
}
