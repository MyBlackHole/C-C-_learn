//
// Created by BlackHole on 2020/7/27.
//

#include <stdio.h>
#include <string.h>
#include <z3.h>

//bool isSubsequence(const char *s, const char *t) {
//    int index = 0;
//    for (int i = 0; s[i] != '\0'; i++) {
//        int status = 0;
//        for (int j = index; t[j] != '\0'; j++) {
//            if (s[i] == t[j]) {
//                status = 1;
//                index = j + 1;
//                continue;
//            }
//        }
//        if (!status) {
//            return false;
//        }
//    }
//    return true;
//}
bool isSubsequence1(char *s, char *t) {
    int n = strlen(s), m = strlen(t);

    int f[m + 1][26];
    memset(f, 0, sizeof(f));
    for (int i = 0; i < 26; i++) {
        f[m][i] = m;
    }

    for (int i = m - 1; i >= 0; i--) {
        for (int j = 0; j < 26; j++) {
            if (t[i] == j + 'a')
                f[i][j] = i;
            else
                f[i][j] = f[i + 1][j];
        }
    }
    int add = 0;
    for (int i = 0; i < n; i++) {
        if (f[add][s[i] - 'a'] == m) {
            return false;
        }
        add = f[add][s[i] - 'a'] + 1;
    }
    return true;
}


bool isSubsequence(char *s, char *t) {
    int n = strlen(s), m = strlen(t);
    int i = 0, j = 0;
    while (i < n && j < m) {
        if (s[i] == t[j]) {
            i++;
        }
        j++;
    }
    return i == n;
}

int main() {
    char str[] = "leeeeetcode";
    char ttr[] = "leeeeetcode";
    char *s = str;
    char *t = ttr;
    bool b = isSubsequence(s, t);
    printf("%d", b);
}