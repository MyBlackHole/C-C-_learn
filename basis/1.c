//
// Created by Black Hole on 2020/10/31.
//
#include <stdio.h>

int main() {
//    printf("hello, word\n");
//    int fahr, celsius;
//    int lower, upper, step;
    float fahr, celsius;
    float lower, upper, step;

    lower = 0;
    upper = 300;
    step = 20;

    fahr = lower;
    while (fahr <= upper) {
        celsius = 5 * (fahr - 32) / 9;
        printf("%3.0f\t%6.1f\n", fahr, celsius);
        fahr = fahr + step;
    }
    return 0;
}
