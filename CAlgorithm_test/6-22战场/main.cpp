#include <cmath>
#include <cstring>
#include <iostream>

#define N 1000+5

using namespace std;

int n, stamp[N];
double cleft, cright;

struct spot {
    int x, y, r;

    void set() {
        scanf("(%d,%d,%d)", &x, &y, &r);
    }
} cround[N];

bool can(spot a, spot b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx * dx + dy * dy - (a.r + b.r) * (a.r + b.r) <= 0;
}

bool dfs(int u) {
    stamp[u] = 1;
    if (cround[u].y - cround[u].r <= 0)
        return false;
    if (cround[u].x - cround[u].r <= 0)
        cleft = min(cleft, cround[u].y - sqrt(cround[u].r * cround[u].r - cround[u].x * cround[u].x));
    if (1000 - cround[u].x - cround[u].r <= 0)
        cright = min(cright,
                     cround[u].y - sqrt(cround[u].r * cround[u].r - (1000 - cround[u].x) * (1000 - cround[u].x)));
    for (int i = 0; i < n; i++)
        if (!stamp[i]) {
            if (can(cround[u], cround[i]))
                if (!dfs(i))
                    return false;
        }
    return true;
}

void solve() {
    for (int i = 0; i < n; i++)
        if (cround[i].y + cround[i].r >= 1000 && !stamp[i])
            if (!dfs(i)) {
                cout << "�޽⣡" << endl;
                return;
            }
    printf("��(0,%.2lf):��(1000,%.2lf)\n", cleft, cright);
}

int main(int argc, char **argv) {
    while (~scanf("%d", &n)) {
        memset(stamp, 0, sizeof(stamp));
        cleft = cright = 1000;
        for (int i = 0; i < n; i++)
            cround[i].set();
        solve();
    }
    return 0;
}
