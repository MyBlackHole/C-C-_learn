//
// Created by BlackHole on 2020/7/26.
//

#include <stdio.h>


const int dirs[4][2] = {{0,  1},
                        {1,  0},
                        {-1, 0},
                        {0,  -1}};

int matrix_bat[3][3] = {{0, 0, 0},
                        {0, 0, 0},
                        {0, 0, 0}};


void dfs(int **matrix, int x, int y, int status) {
    for (int dir = 0; dir < 4; dir++) {
        int xx = x + dirs[dir][0];
        int yy = y + dirs[dir][1];
        if (0 > xx || xx >= 3 || 0 > yy || yy >= 3)
            continue;
        if (matrix[xx][yy] > matrix[x][y]) {
            matrix_bat[xx][yy] = status;
            dfs(matrix, xx, yy, status + 1);
        }
    }
}

void printMatrix(int **matrix, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void printMatrixBat(int matrix[][3], int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int matrix[3][3] = {{9, 9, 4},
                        {6, 6, 8},
                        {2, 1, 1}};
    int *p[3] = {matrix[0], matrix[1], matrix[2]};
    int **pp = p;
//    printMatrix(matrix_bat, 3, 3);
//    longestIncreasingPath(matrix, 9, (int *) 0);
    dfs(pp, 2, 1, 1);
    printMatrixBat(matrix_bat, 3, 3);
    return 0;
}

