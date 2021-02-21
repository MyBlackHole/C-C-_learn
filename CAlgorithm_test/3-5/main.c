#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 30
#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int huan(char *a, char *b) {
    char c = *a;
    *a = *b;
    *b = c;
}


int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[maxn];
    int x;
    int i;
    char c[7];
    for (i = 0; i < 5; i++) {
        scanf("%s", s + (i * 5));
    }
    for (i = 0; i < 25; i++) {
        if (s[i] == '0') {
            x = i;
            break;
        }

    }
    scanf("%s", c);
    for (i = 0; c[i] != '0'; i++) {
        switch (c[i]) {
            case 'A':
                if ((x - 5) >= 0) {
                    huan(&s[x], &s[x - 5]);
                    x = x - 5;
                } else {
                    printf("This puzzle has no final confrguration\n");
                    c[i + 1] = '0';
                }
                break;
            case 'B':
                if ((x + 5) <= 24) {
                    huan(&s[x], &s[x + 5]);
                    x = x + 5;
                } else {
                    printf("This puzzle has no final confrguration\n");
                    c[i + 1] = '0';
                }
                break;
            case 'L':
                if ((x % 5 - 1) >= 0) {
                    huan(&s[x], &s[x - 1]);
                    x = x - 1;
                } else {
                    printf("This puzzle has no final confrguration\n");
                    c[i + 1] = '0';
                }
                break;
            case 'R':
                if ((x % 5 + 1) <= 4) {
                    huan(&s[x], &s[x + 1]);
                    x = x + 1;
                } else {
                    printf("This puzzle has no final confrguration\n");
                    c[i + 1] = '0';
                }
                break;
            default :
                printf("This puzzle has no final confrguration\n");
        }
    }
    for (i = 0; i < 5; i++) {
        printf("%.5s\n", s + (i * 5));
    }
    return 0;
}
