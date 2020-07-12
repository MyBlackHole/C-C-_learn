#include <iostream>
#include <algorithm>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

using namespace std;

const int maxn = 10000;

int main(int argc, char **argv) {
    int n, q, x, a[maxn], kase = 0;
    while (scanf("%d%d", &n, &q) == 2 && n) {
        printf("CASE# %d:\n", ++kase);
        for (int i = 0; i < n; i++) {
            scanf("%d", &a[i]);
        }
        sort(a, a + n);
        while (q--) {
            scanf("%d", &x);
            int p = lower_bound(a, a + n, x) - a;
//			printf("%d 123  %d  123  %d\n", a, lower_bound(a, a+n, x), p);
            if (a[p] == x)
                printf("%d found at %d\n", x, p + 1);
            else
                printf("%d not found\n", x);
        }
    }
    return 0;
}
