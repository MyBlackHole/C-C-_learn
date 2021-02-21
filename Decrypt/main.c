//
// Created by BlackHole on 2020/7/22.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void rotate(char *, int);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s shift message\n", argv[0]);
        return 1;
    }

    int shift = atoi(argv[1]);
    if (shift == 0 || shift > 25) {
        printf("Invalid shift.\n");
        return 2;
    }

    printf("subject: %s Shift: %d\n", argv[2], shift);
    rotate(argv[2], shift);
    printf("Ciphertext: %s\n", argv[2]);
}

void rotate(char *cleartext, int shift) {
    int length = strlen(cleartext);
    char temp;
    int i;    // how do I turn on C99 mode in gcc4? :(
    for (i = 0; i < length; ++i) {
        temp = cleartext[i];
        if (temp >= 65 && temp <= 90) {
            temp = (((temp - 65 + shift) % 26) + 65);
        } else if (temp >= 97 && temp <= 122) {
            temp = (((temp - 97 + shift) % 26) + 97);
        }
        cleartext[i] = temp;
    }
}


void RotN(const char *str, int a, int b) {
    size_t count = strlen(str);
    int j = count + 1;
    int k;
    for (int i = 0; count != i; ++i) {
        k = str[i];
        if ((k - 48) <= 90 && b) {

        }
    }
}
