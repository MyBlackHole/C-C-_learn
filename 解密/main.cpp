//
// Created by BlackHole on 2020/7/22.
//
// Building a simple rotation cipher.  This is just practice
// since my c/c++ skillz are lacking again.
// author: Nick Soracco <nsoracco!AT!cs.cur.edu>
// Licence: public domain

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// pull in a single char and a shift value
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

// By reducing the symbol's ASCII value to it's symbolic from 0 representation
// and then adding the shift value and modulo by the number of symbols in the
// alphabet and then adding the base back in to get the ASCII value of the
// shifted value back.  The easier way (and works with all symbols!) is to do
// something like this: shifted = (((symbol + shift) % symbol ) + offset)
// where:	offset = distance from 0 to the first symbol's ascii value
// 		symbol = ascii value of the symbol in question
//		shift  = how many places up to move.
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