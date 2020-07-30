//
// Created by BlackHole on 2020/7/30.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int integerBreak(int n) {
    int dp[n + 1];
    memset(dp, 0, sizeof(dp));
    for (int i = 2; i <= n; i++) {
        int curMax = 0;
        for (int j = 1; j < i; j++) {
            curMax = fmax(curMax, fmax(j * (i - j), j * dp[i - j]));
        }
        dp[i] = curMax;
    }
    return dp[n];
}

int main() {
    int i = integerBreak(9);
    printf("%d", i);
}