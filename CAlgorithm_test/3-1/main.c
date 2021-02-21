#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 85
#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[maxn];
    int n = 0;
    int a = 0;
    scanf("%s", s);
    int i;
    int y = strlen(s);
    for (i = 0; i < y; i++) {
        if (s[i] == 'O') {
            n++;
            a += n;
        } else {
            n = 0;
        }
    }
    printf("%d", a);
    return 0;
}
