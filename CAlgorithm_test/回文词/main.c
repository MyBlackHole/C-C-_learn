#define LOCAL

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
const char *rev = "A   3  HIL JM O   2TUVMXY51SE Z  8 ";
const char *msg[] = {"not a palindrome", "a regular palindrome", "a mirrored string", "a mirrored palindrome"};

char r(char ch) {
    if (isalpha(ch))
        return rev[ch - 'A'];
    return rev[ch - '0' + 25];
}

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[30];
    while (scanf("%s", s) == 1) {
        int len = strlen(s);
        int p = 1;
        int m = 1;
        int i;
        for (i = 0; i < (len + 1) / 2; i++) {
            if (s[i] != s[len - 1 - i]) p = 0;
            if (r(s[i]) != s[len - 1 - i]) m = 0;
        }
        printf("%s -- is %s.\n\n", s, msg[m * 2 + p]);
    }
    printf("%f", pow(10, 2));
    return 0;
}
