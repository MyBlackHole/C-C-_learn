//
// Created by BlackHole on 2020/7/25.
//
#include <math.h>
#include <stdio.h>
#include <string.h>

int splitArray(int *nums, int numsSize, int m) {
    long long f[numsSize + 1][m + 1];
    memset(f, 0x3f, sizeof(f));
    long long sub[numsSize + 1];
    memset(sub, 0, sizeof(sub));
    for (int i = 0; i < numsSize; i++) {
        sub[i + 1] = sub[i] + nums[i];
    }
    f[0][0] = 0;
    for (int i = 1; i <= numsSize; i++) {
        for (int j = 1; j <= fmin(i, m); j++) {
            for (int k = 0; k < i; k++) {
                f[i][j] = fmin(f[i][j], fmax(f[k][j - 1], sub[i] - sub[k]));
            }
        }
    }
    return (int) f[numsSize][m];
}

int main() {
    int nums[] = {1, 3, 5, 2, 7};
    int i = splitArray(nums, 5, 2);
    printf("%d", i);
    return 0;
}
