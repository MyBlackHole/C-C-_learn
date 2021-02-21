#include<iostream>
#include<string>
#include<cstring>
#include<set>

using namespace std;

void connected(int v);

bool degree_to_meet();

void init();

const int MAXN = 26 + 4;
int g[MAXN][MAXN], in[MAXN], out[MAXN], cnt;
set<int> chr;
bool vis[MAXN];        //DFS

int main() {
    freopen("input.txt", "r", stdin);
    //freopen("output.txt","w",stdout);
    int n, m;
    string s;
    cin >> n;
    init();
    while (n--) {
        cin >> m;
        while (m--) {
            cin >> s;
            int ch_in = s[0] - 'a', ch_out = s[s.size() - 1] - 'a';
            g[ch_in][ch_out] = 1;
            g[ch_out][ch_in] = 1;
            in[ch_in]++;
            out[ch_out]++;
            chr.insert(ch_in);
            chr.insert(ch_out);
        }
        cnt = 1;
        connected(*chr.begin());
        if (cnt == chr.size() && degree_to_meet())
            cout << "Ordering is possible.\n";
        else cout << "The door cannot be opened.\n";
        init();
    }
    return 0;
}

bool degree_to_meet() {
    bool ch_begin = false, ch_end = false;
    for (set<int>::iterator it = chr.begin(); it != chr.end(); it++) {
        int dv = in[*it] - out[*it];
        if (dv) {
            if ((dv != 1 && dv != -1) || (ch_begin && dv == 1) || (ch_end && dv == -1))
                return false;
            if (dv == 1) ch_begin = true;
            if (dv == -1) ch_end = true;
        }
    }
    return true;
}

void connected(int v) {
    vis[v] = true;
    for (set<int>::iterator it = chr.begin(); it != chr.end(); it++) {
        if (g[v][*it] && !vis[*it]) {
            cnt++;
            connected(*it);
        }
    }
    return;
}

void init() {
    chr.clear();
    memset(g, 0, sizeof(g));
    memset(in, 0, sizeof(in));
    memset(out, 0, sizeof(out));
    memset(vis, 0, sizeof(vis));
}

