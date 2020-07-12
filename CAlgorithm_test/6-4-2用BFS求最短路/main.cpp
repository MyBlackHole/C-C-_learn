#include<iostream>
#include<queue>
#include<cstdio>
#include<cstring>
#include<vector>

using namespace std;
const int maxn = 100 + 5;

int G[maxn][maxn];
int path[maxn];
int n, m;
int vy[5] = {1, 0, -1, 0};
int vx[5] = {0, 1, 0, -1};
bool vis[maxn][maxn];

struct node {
    int x;
    int y;
    int id;
    int parent;

    node(int a, int b, int c) {
        y = a;
        x = b;
        id = c;
        parent = 0;
    }
};

int main() {
    //freopen("in.txt","r",stdin);
    memset(G, 0, sizeof(G));
    memset(vis, 0, sizeof(vis));
    memset(path, 0, sizeof(path));
    cin >> n >> m;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++) {
            cin >> G[i][j];
        }
    queue<node> q;
    node v = node(1, 1, 1);
    q.push(v);
    vis[1][1] = 1;
    while (!q.empty()) {
        node u = q.front();
        q.pop();
        path[u.id] = u.parent;
        for (int i = 0; i < 4; i++) {
            int tx = u.x + vx[i];
            int ty = u.y + vy[i];
            if (G[ty][tx] && !vis[ty][tx]) {
                vis[ty][tx] = 1;
                node v = node(ty, tx, G[ty][tx]);
                v.parent = u.id;
                q.push(v);
            }
        }
    }
    vector<int> ans;
    int p = 23;

    while (p) {
        ans.push_back(p);
        p = path[p];
    }
    for (int i = ans.size() - 1; i >= 0; i--) {
        cout << ans[i] << ' ';
    }
    cout << endl;
    return 0;
}
