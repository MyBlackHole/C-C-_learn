#include <stdio.h>
#include <stdlib.h>

#define LOCAL

#include <string.h>
#include <math.h>

#define maxn 85

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
#ifdef LOCAL
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
#endif
    char s[maxn];
    scanf("%s", s);
    int i;
    int b;
    int z = 0;
    for (i = 1; i < strlen(s) / 2; i++) {
        int k;
        for (k = 0; k < i; k++) {
            int j;
            for (j = 1; j < round(strlen(s)) / i; j++) {
                if ((j * i + k) < strlen(s) && s[k] == s[j * i + k])
                    b = 1;
                else {
                    b = 0;
                    break;
                }
            }
            if (b == 1)
                z += 1;
            else {
                z = 0;
                break;
            }
        }
        if (z != 0)
            break;
    }
    if (z == 0)
        printf("%d", strlen(s));
    else
        printf("%d", z);

    return 0;
}
