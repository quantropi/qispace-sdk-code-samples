/*
 * qstdlib.c
 *
 *  Created on: Apr 30, 2024
 *      Author: Xijian
 */

#ifndef QSTDLIB_C_
#define QSTDLIB_C_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include "hal_entry.h"

#define MAXBUF 250
static char dbg[MAXBUF+4];
int Q_printf(const char *format, ...);


int Q_printf(const char *format, ...)
{
    va_list args;
    fsp_err_t err  = FSP_SUCCESS;


    va_start(args, format);
    vsnprintf(dbg, MAXBUF, format, args);
    va_end(args);


    // write to output
    err = print_to_console(dbg);
    if (FSP_SUCCESS != err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON
    }

    return 0;
}


#endif /* QSTDLIB_C_ */
