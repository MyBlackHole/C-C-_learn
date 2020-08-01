//
// Created by BlackHole on 2020/8/1.
//
#include "stdio.h"
#include "string.h"

int getAnswer(int *nums, int left, int right) {
    if (left > right) {
        return -1;
    }
    int mid = (right - left) / 2 + left;
    int leftAnswer = getAnswer(nums, left, mid - 1);
    if (leftAnswer != -1) {
        return leftAnswer;
    } else if (nums[mid] == mid) {
        return mid;
    }
    return getAnswer(nums, mid + 1, right);
}

int findMagicIndex(int *nums, int numsSize) {
    return getAnswer(nums, 0, numsSize - 1);
}

int main() {
    int is[] = {6, 2, 4, 3, 1, 8};
    int *nums = is;
    int i = findMagicIndex(nums, 6);
    printf("%d", i);
}

