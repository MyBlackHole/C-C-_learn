//
// Created by Black Hole on 2020/10/31.
//
#include <stdio.h>

void copy();

void charCount();

void lintCount();

int main() {
    copy();
    charCount();
    lintCount();
    return 0;
}

void copy() {
    int c;
    c = getchar();
    while (c != EOF) {
        putchar(c);
        c = getchar();
    }
}

void charCount() {
    double nc;
    for (nc = 0; getchar() != EOF; ++nc);
    printf("%.0f", nc);
}

void lintCount() {
    int c, nl;
    nl = 0;
    while (c = getchar() != EOF)
        if (c == '\n')
            ++nl;

    printf("%d", nl);
}

#define IN 1
#define OUT 0

void wordCount() {
    int c, nl, nw, nc, state;

    state = OUT;
    nl = nw = nc = 0;
    while ((c = getchar()) != EOF) {
        ++nc;
        if (c == '\n')
            ++nl;
        if (c == ' ' || c == '\n' || c == '\t')
            state = OUT;
        else if (state == OUT) {
            state = IN;
            ++nw;
        }
    }
    printf("%d %d %d\n", nl, nw, nc);
}