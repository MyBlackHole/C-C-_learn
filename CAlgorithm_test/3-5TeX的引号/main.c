#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    int c, q = 1;
    while ((c = getchar()) != EOF) {
        if (c == '"') {
            printf("%s", q ? " 1 " : " 2 ");
            q = !q;
        } else
            printf("%c", c);
    }
    return 0;
}
