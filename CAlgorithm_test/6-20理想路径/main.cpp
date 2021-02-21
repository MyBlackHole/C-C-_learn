#include<iostream>
#include<queue>
#include<cstdio>
#include<set>
#include<cstring>

/*
3 3
1 2 1
2 3 2
1 3 2
1
2
4 4
1 2 1
2 4 3
1 3 4
3 4 1
2
1 3
*/

using namespace std;
struct node {
    int b, e, c;
};

typedef node edge;

const int size = 1e5 + 5;
edge roar[4 * size];
int first[size], nxt[size * 4];
int tot = 0;
int n, m;
set<int> flag;
int len[size];
int befo[size];

void edge_build(int b, int e, int c) {
    roar[tot].b = b;
    roar[tot].e = e;
    roar[tot].c = c;
    nxt[tot] = first[b];
    first[b] = tot++;
}

queue<int> q;

void bfs() {
    flag.clear();
    q.push(n);
    flag.insert(n);
    len[n] = 0;
    while (!q.empty()) {
        int s = q.front();
        q.pop();
        for (int i = first[s]; i != -1; i = nxt[i]) {
            int b = roar[i].b;
            int e = roar[i].e;
            if (flag.count(e)) {
                if (len[s] + 1 == len[e]) {
                    int j = befo[e];
                    int k = i;
                    while (j != k && roar[j].c == roar[k].c) j = befo[roar[j].b], k = befo[roar[k].b];
                    if (roar[j].c > roar[k].c) befo[e] = i;
                }
            } else {
                flag.insert(e);
                befo[e] = i;
                len[e] = len[s] + 1;
                q.push(e);
            }
        }
    }
}

int main() {
    while (~scanf("%d%d", &n, &m)) {
        memset(first, -1, sizeof(first));
        memset(nxt, 0, sizeof(nxt));
        tot = 0;
        for (int i = 0; i < m; i++) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            edge_build(a, b, c);
            edge_build(b, a, c);
        }
        bfs();
        cout << len[1] << endl;
        for (int i = befo[1]; len[1]--; i = befo[roar[i].b]) {
            cout << roar[i].c;
            if (len[1]) printf(" ");
            else printf("\n");
        }
    }
}

