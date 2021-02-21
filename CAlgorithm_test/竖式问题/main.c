#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    int count = 0;
    char s[20], buf[99];
    scanf("%s", s);
    int abc, de;
    for (abc = 111; abc < 999; abc++)
        for (de = 11; de < 99; de++) {
            int x = abc * (de % 10), y = abc * (de / 10), z = abc * de;
            sprintf(buf, "%d%d%d%d%d", abc, de, x, y, z);
            int ok = 1, i;
            for (i = 0; i < strlen(buf); i++)
                if (strchr(s, buf[i]) == NULL)
                    ok = 0;
            if (ok) {
                printf("<%d>\n", ++count);
                printf("%5d\nX%4d\n-----\n%5d\n%4d\n-----\n%5d\n\n", abc, de, x, y, z);
            }
        }
    printf("The number of solutions = %d\n", count);
    return 0;
}
