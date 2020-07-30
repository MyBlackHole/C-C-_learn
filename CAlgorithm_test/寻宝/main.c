//
// Created by BlackHole on 2020/7/30.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z3.h>

const int dx[4] = {1, -1, 0, 0};
const int dy[4] = {0, 0, 1, -1};
int n, m;

bool inBound(int x, int y) { return x >= 0 && x < n && y >= 0 && y < m; }

int **bfs(int x, int y, char **maze) {
    int **ret = (int **) malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++) {
        ret[i] = (int *) malloc(sizeof(int) * m);
        memset(ret[i], -1, sizeof(int) * m);
    }
    ret[x][y] = 0;
    int quex[n * m], quey[n * m];
    quex[0] = x, quey[0] = y;
    int left = 0, right = 0;
    while (left <= right) {
        for (int k = 0; k < 4; k++) {
            int nx = quex[left] + dx[k], ny = quey[left] + dy[k];
            if (inBound(nx, ny) && maze[nx][ny] != '#' && ret[nx][ny] == -1) {
                ret[nx][ny] = ret[quex[left]][quey[left]] + 1;
                quex[++right] = nx, quey[right] = ny;
            }
        }
        left++;
    }
    return ret;
}

typedef struct point {
    int x, y;
} point;

int minimalSteps(char **maze, int mazeSize) {
    n = mazeSize, m = strlen(maze[0]);
    // 机关 & 石头
    point *buttons = (point *) malloc(0);
    point *stones = (point *) malloc(0);
    int nb = 0, ns = 0;
    // 起点 & 终点
    int sx, sy, tx, ty;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (maze[i][j] == 'M') {
                buttons = (point *) realloc(buttons, sizeof(point) * (++nb));
                buttons[nb - 1] = (point) {i, j};
            }
            if (maze[i][j] == 'O') {
                stones = (point *) realloc(stones, sizeof(point) * (++ns));
                stones[ns - 1] = (point) {i, j};
            }
            if (maze[i][j] == 'S') {
                sx = i, sy = j;
            }
            if (maze[i][j] == 'T') {
                tx = i, ty = j;
            }
        }
    }
    int **start_dist = bfs(sx, sy, maze);

    // 边界情况：没有机关
    if (nb == 0) {
        return start_dist[tx][ty];
    }
    // 从某个机关到其他机关 / 起点与终点的最短距离。
    int **dist = (int **) malloc(sizeof(int *) * nb);
    for (int i = 0; i < nb; i++) {
        dist[i] = (int *) malloc(sizeof(int) * (nb + 2));
        memset(dist[i], -1, sizeof(int) * (nb + 2));
    }
    // 中间结果
    int ***dd = (int ***) malloc(sizeof(int **) * nb);
    for (int i = 0; i < nb; i++) {
        int **d = bfs(buttons[i].x, buttons[i].y, maze);
        dd[i] = d;
        // 从某个点到终点不需要拿石头
        dist[i][nb + 1] = d[tx][ty];
    }

    for (int i = 0; i < nb; i++) {
        int tmp = -1;
        for (int k = 0; k < ns; k++) {
            int mid_x = stones[k].x, mid_y = stones[k].y;
            if (dd[i][mid_x][mid_y] != -1 && start_dist[mid_x][mid_y] != -1) {
                if (tmp == -1 || tmp > dd[i][mid_x][mid_y] + start_dist[mid_x][mid_y]) {
                    tmp = dd[i][mid_x][mid_y] + start_dist[mid_x][mid_y];
                }
            }
        }
        dist[i][nb] = tmp;
        for (int j = i + 1; j < nb; j++) {
            int mn = -1;
            for (int k = 0; k < ns; k++) {
                int mid_x = stones[k].x, mid_y = stones[k].y;
                if (dd[i][mid_x][mid_y] != -1 && dd[j][mid_x][mid_y] != -1) {
                    if (mn == -1 || mn > dd[i][mid_x][mid_y] + dd[j][mid_x][mid_y]) {
                        mn = dd[i][mid_x][mid_y] + dd[j][mid_x][mid_y];
                    }
                }
            }
            dist[i][j] = mn;
            dist[j][i] = mn;
        }
    }
    // 无法达成的情形
    for (int i = 0; i < nb; i++) {
        if (dist[i][nb] == -1 || dist[i][nb + 1] == -1) return -1;
    }

    // dp 数组， -1 代表没有遍历到
    int **dp = (int **) malloc(sizeof(int *) * (1 << nb));
    for (int i = 0; i < (1 << nb); i++) {
        dp[i] = (int *) malloc(sizeof(int) * nb);
        memset(dp[i], -1, sizeof(int) * nb);
    }
    for (int i = 0; i < nb; i++) {
        dp[1 << i][i] = dist[i][nb];
    }

    // 由于更新的状态都比未更新的大，所以直接从小到大遍历即可
    for (int mask = 1; mask < (1 << nb); mask++) {
        for (int i = 0; i < nb; i++) {
            // 当前 dp 是合法的
            if (mask & (1 << i)) {
                for (int j = 0; j < nb; j++) {
                    // j 不在 mask 里
                    if (!(mask & (1 << j))) {
                        int next = mask | (1 << j);
                        if (dp[next][j] == -1 || dp[next][j] > dp[mask][i] + dist[i][j]) {
                            dp[next][j] = dp[mask][i] + dist[i][j];
                        }
                    }
                }
            }
        }
    }
    int ret = -1;
    int final_mask = (1 << nb) - 1;
    for (int i = 0; i < nb; i++) {
        if (ret == -1 || ret > dp[final_mask][i] + dist[i][nb + 1]) {
            ret = dp[final_mask][i] + dist[i][nb + 1];
        }
    }

    return ret;
}

int main() {
    char matrix[3][3] = {{'S', '#', 'O'},
                         {'M', '.', '#'},
                         {'M', '.', '.'}};
    char *p[3] = {matrix[0], matrix[1], matrix[2]};
    char **maze = p;
    int i = minimalSteps(maze, 9);
    printf("%d", i);
}
