#include <stdio.h>
#include <stdlib.h>

#define maxn 1010

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int out(int argc, char *argv[]) {
    FILE *fin, *fout;
    fin = fopen("data.in", "r");
    fout = fopen("data.out", "w");
    int n, a[maxn], b[maxn];
    fscanf(fin, "%d", &n);
    fscanf(fin, "%d", &a[0]);
    fscanf(fin, "&d", &b[0]);
    fprintf(fout, " n = %d (%d,%d)\n", n, a[0], b[0]);
    fclose(fin);
    fclose(fout);
    return 0;
}
