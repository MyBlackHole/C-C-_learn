//
// Created by Black Hole on 2020/11/2.
//

#include <stdio.h>
#include <stdarg.h>

#define MAXLINE 2048

void PrintFError(const char *format, ...);

int main() {
    PrintFError("Error");
    return 0;
}


void PrintFError(const char *format, ...) {
    char buf[MAXLINE];
    va_list args;
            va_start(args, format);
    vsnprintf(buf, MAXLINE, format, args);
    perror(buf);
            va_end(args);
}