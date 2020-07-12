#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 10000000000

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    char s[10];
    scanf("%s", s);
    int tot = 0;
    int i;
    //int n = strlen(s);
    for (i = 0; i < strlen(s); i++)
        if (s[i] == '1')
            tot++;
    printf("%d\n", tot);
    return 0;
}
