#include <stdio.h>
#include <string.h>

// 输出所有
void print(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; ++i) {
        printf("%c", a[i]);
    }
    printf("\n");
}


// 冒泡排序
// 时间复杂O(n^2)
// 空间复杂O(3)
void sort(char *a) {
    int i;
    int j;
    for (i = 0; a[i] != '\0'; ++i) {
        for (j = i - 1; a[j] != '\0'; ++j) {
            if (a[i] < a[j]) {
                int z = a[i];
                a[i] = a[j];
                a[j] = z;
                // a[i] = a[i] + a[j];
                // a[j] = a[i] - a[j];
                // a[i] = a[i] - a[j];
            }
        }
    }
}

// 遍历每个字符串
// 时间复杂O(n*m))
// 空间复杂O(2)
int contain(char *s1, int i1, char *s2, int i2) // s1：短，s2：长
{
    int i;
    for (i = 0; s1[i] != '\0'; ++i) {
        int j = 0;
        for (j = 0; s1[i] != s2[j] && s2[j] != '\0'; j++);
        printf("%d\n", j);
        if (j > i2)
            return 0;
    }
    return 1;
}

// 排序好遍历
// 时间复杂O(n) + log(n) + log(m)
// 空间复杂O(2)
int sort_contain(char *s1, int i1, char *s2, int i2) {
    sort(s1);
    sort(s2);

    int i;
    int j = 0;
    for (i = 0; i < i1; ++i)
        for (j; j < i2 && s1[i] != s2[j]; ++j);
    if (j + 1 > i2)
        return 0;
    return 1;
}

// 素数相乘
// 时间复杂O(n + m)
// 空间复杂O(27)
int prime_numbers_mul(char *s1, int i1, char *s2, int i2) {
    int prime[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53};

    int i;
    int f = 1;
    for (i = 0; i < i2; ++i) {
        int x = prime[s2[i] - 'a'];
        if (f % x) {
            f *= x;
        }
    }
    printf("%d", f);
    for (i = 0; i < i1; ++i) {
        int x = prime[s1[i] - 'a'];
        if (f % x) {
            return 0;
        }
    }
    return 1;
}

// 计数排序
// 时间复杂O
// 空间复杂O
int count_sort_contain(char *s1, int i1, char *s2, int i2) {
    return 1;
}

int main(int argc, char *argv[]) {
    int i = 0;
    char s1[] = "bs";
    char s2[] = "abcd";

    // if (contain("da", 2, "abcd", 4) == 1)
    //     printf("是");
    // else
    //     printf("否");

    // if (sort_contain(s1, 2, s2, 4))
    //     printf("是");
    // else
    //     printf("否");

    if (prime_numbers_mul(s1, 2, s2, 4))
        printf("是");
    else
        printf("否");
    return 0;
}
