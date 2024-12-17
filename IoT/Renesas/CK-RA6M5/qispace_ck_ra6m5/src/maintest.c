/*
 * maintest.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Xijian
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int maintest();

extern int ds_demo(void);
extern int kem_demo(void);


int maintest() {
    int ret1, ret2;

    ret1 = ds_demo();
    ret2 = kem_demo();

    if (ret1 == 0 && ret2==0) {
        return 0;
    } else {
        return 1;
    }
}
