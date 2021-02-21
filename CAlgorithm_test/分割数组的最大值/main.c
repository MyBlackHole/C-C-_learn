//
// Created by BlackHole on 2020/7/25.
//
#include <math.h>
#include <stdio.h>
#include <string.h>

// 动态规划
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


// 二分查找 + 贪心
bool check(int *nums, int numsSize, int m, int x) {
    long long sum = 0;
    int cnt = 1;
    for (int i = 0; i < numsSize; i++) {
        if (sum + nums[i] > x) {
            cnt++;
            sum = nums[i];
        } else {
            sum += nums[i];
        }
    }
    return cnt <= m;
}

int splitArray(int *nums, int numsSize, int m) {
    long long left = 0, right = 0;
    for (int i = 0; i < numsSize; i++) {
        right += nums[i];
        if (left < nums[i]) {
            left = nums[i];
        }
    }
    while (left < right) {
        long long mid = (left + right) >> 1;
        if (check(nums, numsSize, m, mid)) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return left;
}


int main() {
    int nums[] = {1, 3, 5, 2, 7};
    int i = splitArray(nums, 5, 2);
    printf("%d", i);
    return 0;
}
