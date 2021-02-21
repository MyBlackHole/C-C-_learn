#include <stdio.h>

#define Max 10


// 输出所有
void print(int arr[], int n) {
    printf("[");
    int i;
    for (i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i != n - 1)
            printf(", ");
    }
    printf("]\n");
}

// 计数
int size(int a[]) {
    int i;
    for (i = 0; a[i] != '\0'; ++i);
    return i;
}

// 快速排序
// 时间复杂O
// 空间复杂O
int partion(int *a, int start, int end) {  // 挖坑填数，2边向中间扫描

    int i = start, j = end;
    int tmp = a[i]; // 这里要做越界检查

    while (i < j) {
        while (i < j && a[j] >= tmp) {
            j--;
        }
        if (i < j) {
            a[i++] = a[j];
        }

        while (i < j && a[i] < tmp) {
            i++;
        }
        if (i < j) {
            a[j++] = a[i];
        }
    }

    //扫描完成后，i==j
    a[i] = tmp;

    return i;
}

void quicksort(int *a, int left, int right) {

    if (left < right)//加上这个，不然有死循环，造成堆栈溢出
    {
        int i = partion(a, left, right);
        quicksort(a, left, i - 1);
        quicksort(a, i + 1, right);
    }
}

// 桶排序
// 时间复杂O
// 空间复杂O
void barrel_sort(int *a) {
}

// 归并排序
// 时间复杂O(logn)
// 空间复杂O(n)
void merge(int array[], int left, int m, int right) {
    int aux[Max] = {0};  // 临时数组
    int i; //第一个数组索引
    int j; //第二个数组索引
    int k; //临时数组索引

    // 分别将 i, j, k 指向各自数组的首部。
    for (i = left, j = m + 1, k = 0; k <= right - left; k++) {
        //若 i 到达第一个数组的尾部，将第二个数组余下元素复制到 临时数组中
        if (i == m + 1) {
            aux[k] = array[j++];
            continue;
        }
        //若 j 到达第二个数组的尾部，将第一个数组余下元素复制到 临时数组中
        if (j == right + 1) {
            aux[k] = array[i++];
            continue;
        }
        //如果第一个数组的当前元素 比 第二个数组的当前元素小，将 第一个数组的当前元素复制到 临时数组中
        //如果第二个数组的当前元素 比 第一个数组的当前元素小，将 第二个数组的当前元素复制到 临时数组中
        if (array[i] < array[j]) {
            aux[k] = array[i++];
        } else {
            aux[k] = array[j++];
        }
    }

    //将有序的临时数组 元素 刷回 被排序的数组 array 中，
    //i = left , 被排序的数组array 的起始位置
    //j = 0， 临时数组的起始位置
    for (i = left, j = 0; i <= right; i++, j++) {
        array[i] = aux[j];
    }
}

void merging_sort(int array[], int start, int end) {
    if (start < end) {
        int i;
        i = (end + start) / 2;
        // 对前半部分进行排序
        merging_sort(array, start, i);
        // 对后半部分进行排序
        merging_sort(array, i + 1, end);
        // 合并前后两部分
        merge(array, start, i, end);
    }
}


// 计数排序
// 时间复杂O(n^2)
// 空间复杂O(n)
void count_sort(int *a) {
    int count[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; a[i] != '\0'; i++)
        ++count[a[i]];
    for (int i = 0; i < 11; ++i)
        for (int j = count[i]; j > 0; j--)
            printf("%d", i);
}

// 冒泡排序
// 时间复杂O(n^2)
// 空间复杂O(3)
void bubble_sort(int *a) {
    int i;
    int j;
    for (i = 0; a[i] != '\0'; ++i)
        for (j = i - 1; a[j] != '\0'; ++j)
            if (a[i] < a[j]) {
                int z = a[i];
                a[i] = a[j];
                a[j] = z;
                // a[i] = a[i] + a[j];
                // a[j] = a[i] - a[j];
                // a[i] = a[i] - a[j];
            }
}

int main(int argc, char *argv[]) {
    int arr_test[Max] = {8, 4, 2, 3, 5, 1, 6, 9, 0, 7};
    // int i1[4] = {2, 3, 1};
    // int i2[5] = {1, 4, 2, 3};
    // count_sort(i);
    // bubble_sort(i);
    // merging_sort(arr_test, 0, Max);
    // print(arr_test, Max);
    // int i2[10] = {159, 267, 0, 1, 6, 722, 6363, 1526, 5, 333};
    // print(i2, 10);
    // fast_sort(i2, 0, 10);
    int i2[4] = {5, 6, 0, 1};
    print(i2, 4);
    fast_sort(i2, 0, 4);
    return 0;
}
