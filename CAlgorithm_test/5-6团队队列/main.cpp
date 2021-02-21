#include <map>
#include <queue>
#include <cstdio>
#include <iostream>

using namespace std;
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

const int maxn = 1000 + 10;

int main(int argc, char **argv) {
    int t, kase = 0;
    while (scanf("%d", &t) == 1 && t) {
        printf("Scanario #%d\n", ++kase);

        map<int, int> team;
        for (int i = 0; i < t; i++) {
            int n, x;
            scanf("%d", &n);
            while (n--) {
                scanf("%d", &x);
                team[x] = i;
            }
        }
        queue<int> q, q2[maxn];
        for (;;) {
            int x;
            char cmd[10];
            scanf("%s", cmd);
            if (cmd[0] == 'S') break;
            else if (cmd[0] == 'D') {
                int f = q.front();
                printf("%d\n", q2[t].front());
                q2[t].pop();
                if (q2[t].empty()) q.pop();
            } else if (cmd[0] == 'E') {
                scanf("%d", &x);
                int t = team[x];
                if (q2[t].empty()) q.push(t);
                q2[t].push(x);
            }
        }
        printf("\n");
    }
    return 0;
}
