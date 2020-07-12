#include <stdio.h>
#include <string.h>

// 遍历每个字符串
// 时间复杂O(n*(m-n)) n：短，m：长
// 空间复杂O(2)
int contain(char *s1, int i1, char *s2, int i2) // s1：短，s2：长
{
    int i;
    for (i = 0; i2 - i >= i1; ++i) {
        int j = 0;
        for (j = 0; s1[j] == s2[i + j] && s1[j] != '\0'; j++);
        if (j == i1)
            return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    int i = 0;
    if (contain("b", 1, "abcd", 4) == 1)
        printf("是");
    else
        printf("否");
    return 0;
}
