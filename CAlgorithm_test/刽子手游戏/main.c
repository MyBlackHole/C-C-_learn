#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 100
#define LOCAL

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int left, chance;
char s[maxn], s2[maxn];
int win, lose;

void guess(char ch) {
    int bad = 1;
    int i;
    for (i = 0; i < strlen(s); i++)
        if (s[i] == ch) {
            left--;
            s[i] = ' ';
            bad = 0;
        }
    if (bad)
        --chance;
    if (!chance)
        lose = 1;
    if (!left)
        win = 1;
}

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    int rnd;
    while (scanf("%d%s%s", &rnd, s, s2) == 3 && rnd != -1) {
        printf("Round %d\n", rnd);
        win = lose = 0;
        left = strlen(s);
        chance = 7;
        int i;
        for (i = 0; i < strlen(s2); i++) {
            guess(s2[i]);
            if (win || lose)
                break;
        }
        if (win)
            printf("You win.\n");
        else if (lose)
            printf("You lose.\n");
        else
            printf("You chickened out.\n");
    }
    return 0;
}
