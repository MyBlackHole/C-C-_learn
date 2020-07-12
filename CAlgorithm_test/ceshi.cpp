//
// Created by black on 2019/11/18.
//
#include <stdio.h>
#include <time.h>

void fu() {
    clock_t start, finish;
    start = clock();
    for (int i = 0; i < 10000000; ++i) { ;
    }
    finish = clock();
    printf("%f", (double) (finish - start) / CLOCKS_PER_SEC);
}


int main(int argc, char *argv[]) {
    fu();
    return 0;
}
