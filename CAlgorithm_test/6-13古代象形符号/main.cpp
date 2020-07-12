#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <string>
#include<sstream>
#include<set>
#include<vector>
#include<stack>
#include<map>
#include<queue>
#include<deque>
#include<cstdlib>
#include <cstdio>
#include<cstring>
#include<cmath>
#include<ctime>
#include<functional>

using namespace std;

#define N 1024
int H, W, ant, black;
char *ch = "WAKJSD";
string s, res, ans[N];
const char *table[] = {"0000", "0001", "0010", "0011",
                       "0100", "0101", "0110", "0111", "1000", "1001", "1010",
                       "1011", "1100", "1101", "1110", "1111"};

void clear() {
    for (int i = 0; i < N; i++)
        ans[i].clear();
}

void dfs(int r, int c, int isp) {
    if (r < 0 || r >= H || c < 0 || c >= W * 4)return;
    if ((isp == 1 || isp == 3) && ans[r][c] != '0')return;
    if (isp == 2 && ans[r][c] != '1' && ans[r][c] != '0')return;
    if (isp == 1)ans[r][c] = '-';
    if (isp == 2 && ans[r][c] == '0') {
        black++;
        dfs(r, c, 3);
    }
    if (isp == 2 && ans[r][c] == '1')
        ans[r][c] = '*';
    if (isp == 3)ans[r][c] = '-';
    dfs(r + 1, c, isp);
    dfs(r, c + 1, isp);
    dfs(r - 1, c, isp);
    dfs(r, c - 1, isp);
}

int main() {
    freopen("1.txt", "r", stdin);
    int rnd = 1;
    while (~scanf("%d%d", &H, &W) && (H || W)) {
        ant = 0;
        res.clear();
        clear();
        for (int i = 0; i < H; i++) {
            cin >> s;
            for (int j = 0; j < W; j++) {
                int t;
                if (isdigit(s[j]))
                    t = s[j] - '0';
                else t = 10 + s[j] - 'a';
                ans[i] += table[t];
            }
        }
        for (int i = 0; i < H; i++) {
            if (ans[i][0] == '0')dfs(i, 0, 1);
            if (ans[i][W * 4 - 1] == '0')dfs(i, W * 4 - 1, 1);
        }
        for (int i = 0; i < W * 4; i++) {
            if (ans[0][i] == '0')dfs(0, i, 1);
            if (ans[H - 1][i] == '0')dfs(H - 1, i, 1);
        }
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W * 4; j++)
                if (ans[i][j] == '1') {
                    black = 0;
                    dfs(i, j, 2);
                    res += ch[black];
                }
        sort(res.begin(), res.end());
        printf("Case %d: ", rnd++);
        cout << res << endl;
    }
    return 0;
}

