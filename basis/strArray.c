//
// Created by Black Hole on 2020/11/1.
//

#include <stdio.h>

#define MAX_LINE 1000

int getLine(char line[], int maxLine);

void copy(char to[], char from[]);

int main() {
    int len;
    int max;
    char line[MAX_LINE];
    char longest[MAX_LINE];

    max = 0;
    while ((len = getLine(line, MAX_LINE)) > 0)
        if (len > max) {
            max = len;
            copy(longest, line);
        }
    if (max > 0)
        printf("%s", longest);
    return 0;
}

int getLine(char s[], int lim) {
    int c;
    int i;

    for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = (char)c;
    if (c == '\n') {
        s[i] = (char)c;
        ++i;
    }
    s[i] = '\0';
    return i;
}

void copy(char to[], char from[]) {
    int i;
    i = 0;
    while ((to[i] = from[i]) != '\0')
        ++i;
}