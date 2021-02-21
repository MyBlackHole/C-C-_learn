//
// Created by BlackHole on 2020/8/1.
//
#include "stdio.h"
#include "stdlib.h"
#include "z3.h"
#include "string.h"
#include "math.h"

#define maxn 100005

int heap[maxn];
int heap_count;
int **rec, *nx;

bool heap_comp(int *first, int *second) {
    return rec[*first][nx[*first]] < rec[*second][nx[*second]];
}

void swap(int *first, int *second) {
    int temp = *second;
    *second = *first;
    *first = temp;
    return;
}

void push(int num) {
    int pos = ++heap_count;
    heap[pos] = num;
    while (pos > 1) {
        if (heap_comp(&heap[pos], &heap[pos >> 1])) {
            swap(&heap[pos], &heap[pos >> 1]);
            pos >>= 1;
        } else
            break;
    }
    return;
}

void pop() {
    int top_num = 1;
    int now;
    swap(&heap[top_num], &heap[heap_count--]);
    while ((now = (top_num << 1)) <= heap_count) {
        if (heap_comp(&heap[now + 1], &heap[now]) && now < heap_count) now++;
        if (heap_comp(&heap[now], &heap[top_num])) {
            swap(&heap[top_num], &heap[now]);
            top_num = now;
        } else
            break;
    }
}

int top() { return heap[1]; }

int *smallestRange(int **nums, int numsSize, int *numsColSize,
                   int *returnSize) {
    heap_count = 0;
    nx = (int *) malloc(sizeof(int) * numsSize);
    memset(nx, 0, sizeof(int) * numsSize);
    rec = nums;

    int rangeLeft = 0, rangeRight = 2147483647;
    int minValue = 0, maxValue = -2147483648;
    for (int i = 0; i < numsSize; ++i) {
        push(i);
        maxValue = fmax(maxValue, nums[i][0]);
    }

    while (true) {
        int row = top();
        pop();
        minValue = nums[row][nx[row]];
        if (maxValue - minValue < rangeRight - rangeLeft) {
            rangeLeft = minValue;
            rangeRight = maxValue;
        }
        if (nx[row] == numsColSize[row] - 1) {
            break;
        }
        ++nx[row];
        maxValue = fmax(maxValue, nums[row][nx[row]]);
        push(row);
    }
    int *ret = malloc(sizeof(int) * 2);
    ret[0] = rangeLeft, ret[1] = rangeRight;
    *returnSize = 2;
    return ret;
}

int main() {
    int nums1[] = {4, 10, 15, 24, 26};
    int nums2[] = {0, 9, 12, 20};
    int nums3[] = {5, 18, 22, 30};
    int *p1 = nums1;
    int *p2 = nums2;
    int *p3 = nums3;
    int *pp[] = {p1, p2, p3};
    int **ppp = pp;
    int returnSize = 0;
//    int *ret = smallestRange(ppp, , &returnSize);
    printf("%d", ppp[0][0]);
}
