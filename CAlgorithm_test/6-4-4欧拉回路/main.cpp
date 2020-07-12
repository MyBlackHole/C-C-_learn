#include <iostream>
#include <cstring>

#define LEN 1001

using namespace std;

bool visited[LEN];
bool arc[LEN][LEN];
int degree[LEN];
int n, m;

void dfs(int v) {
    visited[v] = true;
    for (int i = 1; i <= n; i++) {
        if (!visited[i] && arc[v][i]) {
            dfs(i);
        }
    }
}

bool isConnected() {
    for (int i = 1; i <= n; i++) {
        if (!visited[i]) { return false; }
    }
    return true;
}

bool isCircuit() {
    for (int i = 1; i <= n; i++) {
        if (degree[i] % 2) { return false; }
    }
    return true;
}

int main() {
    freopen("in.txt", "r", stdin);
    int i, p, q;
    while (1) {
        memset(visited, 0, LEN);
        memset(arc, 0, sizeof(bool) * LEN * LEN);
        memset(degree, 0, sizeof(int) * LEN);
        cin >> n >> m;
        if (n == 0)break;
        for (i = 0; i < m; i++) {
            cin >> p >> q;
            degree[p]++;
            degree[q]++;
            arc[p][q] = arc[q][p] = true;
        }
        dfs(1);
        if (!isConnected()) { cout << 0 << endl; }
        else {
            if (isCircuit())cout << 1 << endl;
            else cout << 0 << endl;
        }
    }
    return 0;
}

