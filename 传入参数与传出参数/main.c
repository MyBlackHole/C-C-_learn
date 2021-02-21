//
// Created by black on 2020/10/2.
//

#include <stdio.h>
#include "populator.h"
#include "redirect_ptr.h"
#include "para_allocator.h"

int main(void) {
//    unit_t u;
//
//    set_unit(&u);
//    printf("number: %d\nmsg:%s\n", u.number, u.msg);

//    const char *firstday = NULL;
//    const char *secondday = NULL;
//    get_a_day(&firstday);
//    get_a_day(&secondday);
//    printf("%s\t%s\n", firstday, secondday);

    unit_t1 *p = NULL;
    alloc_unit(&p);
    printf("number:%d\nmsg:%s\n", p->number, p->msg);
    free_unit(p);
    p = NULL;
    return 0;
}

