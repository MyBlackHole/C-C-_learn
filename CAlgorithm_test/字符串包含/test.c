#include "stdio.h"

int is_wait(const char *str1, char *str2) {
    for (int i = 0; str2[i] != '\0'; i++) {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    char a[5] = {'a', 'b', 'c', 'd', 'c'};
    char *str = a;
    char *str1 = str + 1;
    for (int i = 0; str[i] != '\0'; i++)
        printf("%c", str[i]);
    return 0;
}
