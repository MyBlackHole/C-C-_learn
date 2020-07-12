#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOCAL
#define maxn 100005
int ans[maxn];

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    int T, n;
    memset(ans, 0, sizeof(ans));
    int m;
    for (m = 1; m < maxn; m++) {
        int x = m, y = m;
        while (x > 0) {
            y += x % 10;
            x /= 10;
        }
        if (ans[y] == 0 || m < ans[y])
            ans[y] = m;
    }
    scanf("%d", &T);
    while (T--) {
        scanf("%d", &n);
        printf("%d\n", ans[n]);
    }
    return 0;
}
