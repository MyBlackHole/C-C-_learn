//
// Created by black on 2020/10/2.
//

#include <string.h>
#include "populator.h"

void set_unit(unit_t *p) {
    if (p == NULL)
        return;
    p->number = 3;
    strcpy(p->msg, "Hello world!");
}
