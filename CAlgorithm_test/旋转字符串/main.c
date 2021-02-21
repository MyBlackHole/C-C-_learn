#include <stdio.h>

void print(char *str, int num) {
    int i;
    for (i = 0; i < num; ++i) {
        printf("%c", str[i]);
    }
}

// 循环移动一个元素
// 时间复杂度O(m * n)
// 空间复杂度O(1)
void mobile(char *str, int num) {
    char s = str[0];
    int i;
    for (i = 1; i < num; ++i) {
        str[i - 1] = str[i];
    }
    str[i - 1] = s;
}

void mobiles(char *str, int num, int i) {
    // 个数
    while (i--) {
        mobile(str, num);
    }
}

// 旋转字符
// 时间复杂度O(n)
// 空间复杂度O(1)
void Reverse(char *s, int lnum, int rnum) {
    // 反转
    while (lnum < rnum) {
        char t = s[lnum];
        s[lnum++] = s[rnum];
        s[rnum--] = t;
    }
}

void RotateStion(char *s, int m, int n) {
    m %= n;
    Reverse(s, 0, m - 1);
    Reverse(s, m, n - 1);
    Reverse(s, 0, n - 1);
}

int main(int argc, char *argv[]) {
    char str[10] = {'d', 'a', 'b', 't', 'd', 'a', 'b', 't', 'k', 'k'};

    // mobile(str, 10);
    mobiles(str, 10, 3);

    RotateStion(str, 3, 10);
    print(str, 10);
    return 0;
}
