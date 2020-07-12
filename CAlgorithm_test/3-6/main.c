#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 105
#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[maxn];
    int a;
    int b;
    scanf("%d %d", a, b);
    for (int i = 0; i < a; i++) {
        scanf("%s", s + (i * b));
    }
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == '0') {
            int x = i;
        }

    }
    return 0;
}
