#include "stdio.h"

//#define HASH (1 << 4) - 1


//int hash_func(const int value) {
//    return value & HASH;
//}

int str_int(const char *str, int start, int end) {
    printf("\nstr_int start: %d, end: %d, str: %s\n", start, end, str);
    int value = 0;
    for (int i = start; i < end; i++) {
        value += str[i];
    }

    printf("str_int value: %d\n\n", value);
    return value;
}

int is_wait(const char *str1, const char *str2) {
    printf("is_wait str1: %s, str2: %s\n", str1, str2);
    for (int i = 0; str2[i] != '\0'; i++) {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

int str_match(const char *str1, int len1, const char *str2, int len2) {
    int value2 = str_int(str2, 0, len2);
    printf("str_match value2: %d\n", value2);
    // int key2 = hash_func(value2);
    for (int i = 0; i < len1 - len2; i++) {
        if (str_int(str1, i, len2 + i) == value2) {
            if (is_wait(str1 + i, str2)) {
                return i;
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char str2[4] = {'b', 'c', 'd', '\0'};
    char str1[6] = {'a', 'b', 'c', 'd', 'e', '\0'};
    int x = str_match(str1, sizeof(str1) - 1, str2, sizeof(str2) - 1);
    // int x = str_match("ab_de", sizeof("ab_de") - 1, "bcd", sizeof("bcd") - 1);
    printf("%d\n", x);

    // // 字符串匹配测试
    // printf("**************\n");
    // printf("字符串匹配测试\n");
    // printf("%d\n", is_wait("abd", "abc"));
    // printf("%d\n", is_wait("abc", "abc"));
    // printf("**************\n");
    //
    // printf("\n");
    //
    // // 字符串value计算测试
    // printf("**************\n");
    // printf("字符串value计算测试\n");
    // printf("%d\n", str_int(str, 3, 4));
    // printf("**************\n");
    return 0;
}
