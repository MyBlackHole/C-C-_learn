#include <cstring>
#include <cctype>
#include <cstdio>
#include <iostream>

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

const int maxn = 200 + 10;
int n;
char buf[maxn][maxn];

void dfs(int r, int c) {
    printf("%c(", buf[r][c]);
    if (r + 1 < n && buf[r + 1][c] == '|') {
        int i = c;
        while (i - 1 >= 0 && buf[r + 2][i - 1] == '-') i--;
        while (buf[r + 2][i] == '-' && buf[r + 3][i] != '\0') {
            if (!isspace(buf[r + 3][i]))
                dfs(r + 3, i);
            i++;
        }
    }
    printf(")");
}

void solve() {
    n = 0;
    for (;;) {
        fgets(buf[n], maxn, stdin);
        if (buf[n][0] == '#')
            break;
        else
            n++;
    }
    printf("(");
    if (n) {
        for (int i = 0; i < strlen(buf[0]); i++)
            if (buf[0][i] != ' ') {
                dfs(0, i);
                break;
            }
    }
    printf(")\n");
}

int main(int argc, char **argv) {
    freopen("in.txt", "r", stdin);
    int T;
    fgets(buf[0], maxn, stdin);
    sscanf(buf[0], "%d", &T);
    while (T--) {
        solve();
        memset(buf, ' ', sizeof(buf));
    }
    return 0;
}
