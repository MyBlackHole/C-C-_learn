#include <stdio.h>
#include <stdlib.h>

#define LOCAL

#include <string.h>

#define maxn 10005

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[maxn];
    char j[11];
    memset(j, 0, sizeof(j));
    scanf("%s", s);
    int i;
    printf("$$$:");
    for (i = 0; i < strlen(s); i++) {
        switch (s[i]) {
            case '0':
                j[0] += 1;
                break;
            case '1':
                j[1] += 1;
                break;
            case '2':
                j[2] += 1;
                break;
            case '3':
                j[3] += 1;
                break;
            case '4':
                j[4] += 1;
                break;
            case '5':
                j[5] += 1;
                break;
            case '6':
                j[6] += 1;
                break;
            case '7':
                j[7] += 1;
                break;
            case '8':
                j[8] += 1;
                break;
            case '9':
                j[9] += 1;
                break;
            default :
                printf("%c", s[i]);
        }
    }
    printf("\n");
    int a;
    for (a = 0; a < 10; a++) {
        printf("%d: %d\n", a, j[a]);
    }
    return 0;
}
