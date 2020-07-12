#include <stdio.h>
#include <stdlib.h>

#define LOCAL
#define maxn 8
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

char s[maxn];
char s1[maxn];
int b = 0;
int i;

void fuzhi(char c[]) {
    int j;
    for (j = 0; j < 8; j++)
        s1[j] = c[j];
    printf("%s  %s%s\n", s, s1, c);
}

int pan(char c[]) {
    int j;
    for (j = 0; j < 8; j++)
        if (s[j] != c[j])
            return 0;
    printf("\n");
    printf("%s%s\n", s, c);
    return 1;
}

int up() {
    char c[] = {s1[1], s1[5], s1[2], s1[3], s1[0], s1[4]};
    b = pan(c);
    fuzhi(c);
    return b;
}

int down() {
    char c[] = {s1[4], s1[0], s1[2], s1[3], s1[5], s1[1]};
    b = pan(c);
    fuzhi(c);
    return b;
}

int letf() {
    char c[] = {s1[0], s1[3], s1[1], s1[4], s1[2], s1[5]};
    b = pan(c);
    fuzhi(c);
    return b;
}

int right() {
    char c[] = {s1[0], s1[2], s1[4], s1[1], s1[3], s1[5]};
    b = pan(c);
    fuzhi(c);
    return b;
}

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    scanf("%s", s);
    scanf("%s", s1);
    for (i = 0; i < 3; i++) {
        if (up())
            return 0;
    }
    for (i = 0; i < 3; i++) {
        if (down())
            return 0;
    }
    for (i = 0; i < 3; i++) {
        if (letf())
            return 0;
    }
    for (i = 0; i < 3; i++) {
        if (right())
            return 0;
    }
    printf("$$");
    return 0;
}
