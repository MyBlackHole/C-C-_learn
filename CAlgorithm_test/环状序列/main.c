#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 105
#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int less(const char *s, int p, int q) {
    int n = strlen(s);
    int i;
    for (i = 0; i < n; i++)
        if (s[(p + i) % n] != s[(q + i) % n])
            return s[(p + i) % n] < s[(q + i) % n];
    return 0;
}

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    int T;
    char s[maxn];
    scanf("%d", &T);
    while (T--) {
        scanf("%s", s);
        int ans = 0;
        int n = strlen(s);
        int i;
        for (i = 0; i < n; i++)
            if (less(s, i, ans))
                ans = i;
        int j;
        for (j = 0; j < n; j++)
            putchar(s[(j + ans) % n]);
        putchar('\n');
    }
    return 0;
}
