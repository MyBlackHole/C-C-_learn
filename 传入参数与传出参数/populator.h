//
// Created by black on 2020/10/2.
//

#ifndef C_POPULATOR_H
#define C_POPULATOR_H
typedef struct {
    int number;
    char msg[20];
} unit_t;

extern void set_unit(unit_t *);
#endif //C_POPULATOR_H
