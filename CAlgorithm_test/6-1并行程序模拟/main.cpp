#include<iostream>
#include<string>
#include<string.h>
#include<cstdio>
#include<deque>
#include<vector>
#include<queue>

using namespace std;

const int MAXN = 1005;
bool lock;
deque<int> qr;
queue<int> qw;
vector<string> sta[MAXN];
int var[26], p[MAXN], t[MAXN];
int Q;

void run(int i) {
    int rt = Q, v;
    string cur;
    while (rt > 0) {
        cur = sta[i][p[i]];
        if (cur[2] == '=') {
            rt -= t[0];
            v = cur[4] - '0';
            if (cur.size() == 6)v = v * 10 + cur[5] - '0';
            var[cur[0] - 'a'] = v;
        } else if (cur[2] == 'i') {
            rt -= t[1];
            printf("%d: %d\n", i, var[cur[6] - 'a']);
        } else if (cur[2] == 'c') {
            rt -= t[2];
            if (lock) {
                qw.push(i);
                return;
            } else lock = true;
        } else if (cur[2] == 'l') {
            lock = false;
            rt -= t[3];
            if (!qw.empty()) {
                v = qw.front();
                qw.pop();
                qr.push_front(v);
            }
        } else return;
        ++p[i];
    }
    qr.push_back(i);
}

int main() {
    int cas;
    scanf("%d", &cas);
    while (cas--) {
        int n;
        scanf("%d", &n);
        for (int i = 0; i < 5; i++)
            scanf("%d", &t[i]);
        scanf("%d", &Q);
        string s;
        for (int i = 1; i <= n; i++) {
            sta[i].clear();
            while (getline(cin, s)) {
                if (s == "")continue;
                sta[i].push_back(s);
                if (sta[i].back() == "end")break;
            }
            qr.push_back(i);
        }
        memset(p, 0, sizeof(p));
        memset(var, 0, sizeof(var));
        while (!qr.empty()) {
            int cur = qr.front();
            qr.pop_front();
            run(cur);
        }
        if (cas) printf("\n");
    }
    return 0;
}

