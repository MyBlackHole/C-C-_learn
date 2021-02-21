//
// Created by black on 2020/10/2.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "para_allocator.h"

void alloc_unit(unit_t1 **pp) {
    unit_t1 *p = malloc(sizeof(unit_t1));
    if (p == NULL) {
        printf("out of memory\n");
        exit(1);
    }
    p->number = 3;
    p->msg = malloc(20);
    strcpy(p->msg, "Hello World!");
    *pp = p;
}

void free_unit(unit_t1 *p) {
    free(p->msg);
    free(p);
}