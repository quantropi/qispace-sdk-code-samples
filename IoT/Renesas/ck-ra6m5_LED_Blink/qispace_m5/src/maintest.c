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
extern int qeep_demo(void);
extern int sequr_demo(void);

int maintest() {
    uint32_t  kem_p, kem_e, kem_d;
    int ret1, ret2, ret3, ret4;

    ret1 = ds_demo();
    ret2 = kem_demo();
    ret3 = qeep_demo();
    ret4 = sequr_demo();

    if (ret1 == 0 && ret2==0 && ret3==0 && ret4==0) {
        return 0;
    } else {
        return 1;
    }
}
