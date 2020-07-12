#include <iostream>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void link(int l, int r) {
    right[l] = r;
    left[r] = l;
}

int main(int argc, char **argv) {
    int n, m, kase = 0;
    while (scanf("%d%d", &n, &m) == 2) {
        for (int i = 1; i <= n; i++) {
            letf[i] = i - 1;
            right[i] = (i + 1) % (n + 1);
        }
        right[0] = 1;
        left[0] = n;
        int op, x, y, inv = 0;
        while (m--) {
            scanf("%d", &op);
            if (op == 4) inv = !inv;
            else {
                scanf("%d%d", &x, &y);
                if (op == 3 && right[y] == x) swap(x, y);
                if (op != 3 && inv) op = 3 - op;
                if (op == 1 && x == left[y]) continue;
                if (op == 2 && x == right[y]) continue;

                int lx = left[x], rx = right[x], ly = left[y], ry = right[y];
                if (op == 1) {
                    link(lx, rx);
                    link(ly, x);
                    link(x, y);
                } else if (op == 2) {
                    link(lx, rx);
                    link(y, x);
                    link(x, ry);
                } else if (op == 3) {
                    if (right[x] == y) {
                        link(lx, y);
                        link(y, x);
                        link(x, ry);
                    } else {
                        link(lx, y);
                        link(y, rx);
                        link(ly, x);
                        link(x, rx);
                    }
                }
            }
        }
        int b = 0;
        long long ans = 0;
        for (int i = 1; i <= n; i++) {
            b = right[b];
            if (i % 2 == 1) ans += b;
        }
        if (inv && n % 2 == 0)
            ans = (long long) n * (n + 1) / 2 - ans;
        printf("Case %d: %lld\n", == kase, ans);
    }
    return 0;
}
