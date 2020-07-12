#include <stdio.h>
#include <stdlib.h>

#define LOCAL

#include <string.h>
#include <math.h>

#define maxn 105

char s[maxn];
double a = 0;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int jisuan(int i, double b) {
    i++;
    int j = i;
    double n = 0;
    if (s[i] <= '9' && s[i] != '\0') {
        while (s[i] <= '9' && s[i] != '\0') {
            i += 1;
            n += 1;
        }
    } else {
        //printf("%f\n", pow(10,0));
        a += b;
    }

    for (; n > 0;) {
        n -= 1;
        //printf("%f\n", pow(10,n));
        a += b * (s[j] - '0') * pow(10, n);
        j += 1;
    }
    return i;
}

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif

    int i;
    scanf("%s", s);
    for (i = 0; i < strlen(s);) {
        if (s[i] == 'C') {
            i = jisuan(i, 12.01);
            //printf("c a = %f\n", a);
            continue;
        }

        if (s[i] == 'H') {
            i = jisuan(i, 1.008);
            //printf("H a = %f\n", a);
            continue;
        }

        if (s[i] == 'O') {
            i = jisuan(i, 16.00);
            //printf("O a = %f\n", a);
            continue;
        }

        if (s[i] == 'N') {
            i = jisuan(i, 14.01);
            //printf("N a = %f\n", a);
            continue;
        }

    }
    printf("������:%0.3fg/mol", a);
    return 0;
}
