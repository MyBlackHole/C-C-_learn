//
// Created by BlackHole on 2020/8/4.
//

#include <z3.h>
#include <memory.h>
#include "stdlib.h"

int **edges;
int *edgeColSize;
int *visited;
bool valid;

void dfs(int u) {
    visited[u] = 1;
    for (int i = 0; i < edgeColSize[u]; ++i) {
        if (visited[edges[u][i]] == 0) {
            dfs(edges[u][i]);
            if (!valid) {
                return;
            }
        } else if (visited[edges[u][i]] == 1) {
            valid = false;
            return;
        }
    }
    visited[u] = 2;
}

bool canFinish(int numCourses, int **prerequisites, int prerequisitesSize, int *prerequisitesColSize) {
    valid = true;
    edges = (int **) malloc(sizeof(int *) * numCourses);
    for (int i = 0; i < numCourses; i++) {
        edges[i] = (int *) malloc(0);
    }
    edgeColSize = (int *) malloc(sizeof(int) * numCourses);
    memset(edgeColSize, 0, sizeof(int) * numCourses);
    visited = (int *) malloc(sizeof(int) * numCourses);
    memset(visited, 0, sizeof(int) * numCourses);
    for (int i = 0; i < prerequisitesSize; ++i) {
        int a = prerequisites[i][1], b = prerequisites[i][0];
        edgeColSize[a]++;
        edges[a] = (int *) realloc(edges[a], sizeof(int) * edgeColSize[a]);
        edges[a][edgeColSize[a] - 1] = b;
    }
    for (int i = 0; i < numCourses && valid; ++i) {
        if (!visited[i]) {
            dfs(i);
        }
    }
    for (int i = 0; i < numCourses; i++) {
        free(edges[i]);
    }
    free(edges);
    free(edgeColSize);
    free(visited);
    return valid;
}