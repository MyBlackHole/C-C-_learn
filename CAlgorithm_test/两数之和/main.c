//
// Created by BlackHole on 2020/7/23.
//

#include <stdio.h>
#include <stdlib.h>

int main() {
    int nums[] = {3, 2, 4};
    int count = 3;
    int target = 6;
    int *res = (int *) malloc(sizeof(int) * 2);
    int status = 0;
    for (int i = 0; i < count - 1; i++) {
        res[0] = nums[i];
        int sum = target - res[0];
        for (int j = i + 1; j < count; j++) {
            if (sum == nums[j]) {
                printf("[%d, %d]", i, j);
                status = 1;
            }
        }

        if (status) {
            break;
        }
    }
    return 0;
}