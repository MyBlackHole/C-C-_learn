//
// Created by BlackHole on 2020/7/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int dirs[4][2] = {{-1, 0},
                        {1,  0},
                        {0,  -1},
                        {0,  1}};
int rows, columns;

int dfs(int **matrix, int row, int column, int **memo);

int longestIncreasingPath(int **matrix, int matrixSize, int *matrixColSize) {
    if (matrixSize == 0 || matrixColSize[0] == 0) {
        return 0;
    }
    rows = matrixSize;
    columns = matrixColSize[0];

    int **memo = (int **) malloc(sizeof(int *) * rows);
    for (int i = 0; i < rows; i++) {
        memo[i] = (int *) malloc(sizeof(int) * columns);
        memset(memo[i], 0, sizeof(int) * columns);
    }
    int ans = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            ans = fmax(ans, dfs(matrix, i, j, memo));
        }
    }
    free(memo);
    return ans;
}

int dfs(int **matrix, int row, int column, int **memo) {
    if (memo[row][column] != 0) {
        return memo[row][column];
    }
    ++memo[row][column];
    for (int i = 0; i < 4; ++i) {
        int newRow = row + dirs[i][0], newColumn = column + dirs[i][1];
        if (newRow >= 0 && newRow < rows && newColumn >= 0 && newColumn < columns &&
            matrix[newRow][newColumn] > matrix[row][column]) {
            memo[row][column] = fmax(memo[row][column], dfs(matrix, newRow, newColumn, memo) + 1);
        }
    }
    return memo[row][column];
}

int main() {
    int matrix[3][3] = {{9, 9, 4},
                        {6, 6, 8},
                        {2, 1, 1}};
    int *p[3] = {matrix[0], matrix[1], matrix[2]};
    int **pp = p;
    int matrixColSize[] = {3, 3};
    int ans = longestIncreasingPath(pp, 9, matrixColSize);
    printf("%d", ans);
}