#include <stdio.h>

// 计数
int size(int *a) {
    int i;
    for (i = 0; a[i] != '\0'; ++i);
    return i;
}

// 输出所有
void print(int *arr, int n) {
    printf("[");
    int i;
    for (i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i != n - 1)
            printf(", ");
    }
    printf("]\n");
}

void divide(int *a, int ia, int *b, int ib) {
    int i = ia + ib;
    int *a1;
    int *b1;
    int num = i / 2;
    a1 = a;
    b1 = b;
    int i1[i];
    printf("%d\n", i);
    for (int i3 = 0, i4 = 0, j = 0; j < i; ++j) {
        if (i3 == num) {
            i1[j] = b1[i4];
            ++i4;
        } else if (i4 == i - num) {
            i1[j] = a1[i3];
            ++i3;
        } else if (a1[i3] < b1[i4]) {
            i1[j] = a1[i3];
            ++i3;
        } else {
            i1[j] = b1[i4];
            ++i4;
        }
    }
    i1[i] = '\0';
//     print(a);
}

// 快
void fast_sort(int *a, int num1, int num2) {
    int i1 = a[num1];
    int index = num1;
    int start_end[2] = {num1, num2 - 1};
    int ii = num1;
    for (int i = num1 + 1; start_end[0] <= start_end[1]; ++i) {
        int i2 = i % 2;
        if (a[start_end[i2]] <= i1) {
            if (start_end[i2] > index) {
                if (i2 == 1) {
                    a[index] = a[start_end[i2]];
                    index = start_end[i2];
                    --start_end[i2];
                    ++ii;
                }
            } else {
                if (i2 == 1) {
                    --start_end[i2];
                } else {
                    ++start_end[i2];
                }
            }
        } else {
            if (start_end[i2] <= index) {
                if (i2 == 0) {
                    a[index] = a[start_end[i2]];
                    index = start_end[i2];
                    ++start_end[i2];
                    ++ii;
                }
            } else {
                if (i2 == 1) {
                    --start_end[i2];
                }
            }

        }
    }
    a[index] = i1;
    if (ii > 0) {
        if ((index - num1) > 0) {
            fast_sort(a, num1, index);
        }
        if ((num2 - index) > 0) {
            fast_sort(a, index + 1, num2);
        }
    }
}

int main(int argc, char *argv[]) {
    // int i[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // int *i1 = &i[4];
    // int *i2 = &i[2];
    // printf("%d %d", i[4] - i[2], (i1 - i2));
    // printf("%d", i[4] - i[2]);
    // printf("%d\n", (int)(i1 - i2));
    // printf("%d\n", *i1);
    // printf("%d\n", i1[0]);
    int i1[4] = {9, 3, 1, 8};
    int i2[10] = {6, 7, 9, 2, 4, 1, 5, 8, 9, 3};
    // printf("%d", size(i2));
    // divide(i1, size(i1), i2, size(i2));
    print(i2, 10);
    fast_sort(i2, 0, 10);
    print(i2, 10);
    return 0;
}
