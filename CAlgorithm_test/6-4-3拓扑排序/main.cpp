#include <iostream>
#include <cstring>

using namespace std;
const int MAXN = 100;
int n, m, G[MAXN][MAXN];
int c[MAXN];
int topo[MAXN], t;


bool dfs(int u) {
    c[u] = -1;
    for (int v = 0; v < n; v++) {
        if (G[u][v]) {
            if (c[v] < 0) {
                return false;
            } else {
                if (!c[v] && !dfs(v))
                    return false;
            }
        }
    }
    c[u] = 1;
    topo[--t] = u;
    return true;
}

bool topoSort() {
    t = n;
    memset(c, 0, sizeof(c));
    for (int i = 0; i < n; ++i) {
        if (!c[i]) {
            if (!dfs(i)) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    freopen("in.txt", "r", stdin);
    cin >> n >> m;
    memset(G, 0, sizeof(G));
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        G[u][v] = 1;
    }
    if (topoSort()) {
        for (int i = 0; i < n; ++i) {
            cout << topo[i] << " ";
        }
        cout << endl;
    } else {
        cout << -1 << endl;
    }
    return 0;
}
