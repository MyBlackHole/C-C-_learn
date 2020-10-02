//
// Created by black on 2020/10/2.
//

#ifndef C_PARA_ALLOCATOR_H
#define C_PARA_ALLOCATOR_H

typedef struct {
    int number;
    char *msg;
} unit_t1;

extern void alloc_unit(unit_t1 **);

extern void free_unit(unit_t1 *)

#endif //C_PARA_ALLOCATOR_H
