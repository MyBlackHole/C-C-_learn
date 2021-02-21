#include <iostream>
#include <cstring>

#define CLEAR(a, b) memset(a, b, sizeof(a))
#define maxn 55

using namespace std;

//-------------------------CHC------------------------------//
int G[maxn][maxn];
int vis[maxn];

int id(char a, char b) {
    int ret = a - 'A';
    return (b == '+') ? (ret * 2 + 1) : ret * 2;
}

void link(char s1, char s2, char s3, char s4) {
    if (s1 == '0' || s3 == '0') return;
    int u = id(s1, s2) ^1, v = id(s3, s4);
    G[u][v] = 1;
}

bool dfs(int u) {
    vis[u] = -1;
    for (int i = 0; i < 52; ++i) {
        if (G[u][i]) {
            if (vis[i] == -1) return true;
            else if (!vis[i] && dfs(i)) return true;
        }
    }
    vis[u] = 1;
    return false;
}

bool have_circle() {
    CLEAR(vis, 0);
    for (int i = 0; i < 52; ++i) {
        if (!vis[i] && dfs(i)) return true;
    }
    return false;
}

int main() {
    int n;
    while (~scanf("%d", &n) && n) {
        CLEAR(G, 0);
        char s[10];
        for (int i = 0; i < n; ++i) {
            scanf("%s", s);
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    if (k == j) continue;
                    link(s[j << 1], s[j << 1 | 1], s[k << 1], s[k << 1 | 1]);
                }
            }
        }
        puts(have_circle() ? "unbounded" : "bounded");
    }
    return 0;
}

