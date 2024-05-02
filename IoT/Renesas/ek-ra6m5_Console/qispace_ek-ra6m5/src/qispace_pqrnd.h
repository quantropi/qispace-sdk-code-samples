/******************************************************************************
 *
 * Quantropi CONFIDENTIAL
 * ----------------------
 *
 * This file is proprietary code and subject to the terms and conditions
 * defined in the file 'LICENSE', which is part of this source code package.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Quantropi Incorporated and its suppliers,
 * if any. The intellectual and technical concepts contained
 * herein are proprietary to Quantropi Incorporated and its suppliers and
 * may be covered by Canadian, U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Quantropi Incorporated.
 *
 *****************************************************************************/

#ifndef _QISPACE_SDK_PQRND_H
#define _QISPACE_SDK_PQRND_H

# ifdef  __cplusplus
extern "C" {
# endif

#include <stdint.h>
#include "error.h"

/* QiSpace PQRND APIs */

typedef struct QISPACE_SDK_Core_Handle_   *pQrnd_Handle;


/*
* Initialize SDK Core Handle
* Return QEEP_OK if successful
*/
QEEP_RET pQrndInit(pQrnd_Handle *pQrnd);


/*
* Set pQrnd seed 
* Input parameters:
*        - pQrnd: handle created by pQrnd_Init
*        - seed: seed for pQrnd
*        - len: seed buffer length in bytes
* Return QEEP_OK if successful
*/
QEEP_RET pQrndSeed(pQrnd_Handle pQrnd, uint8_t *seed, int32_t len);


/*
* Generate PQRND random 
* Input parameters:
*        - pQrnd: handle created by pQrnd_Init
*        - rnd: random value
*        - len: random length to generate in bytes
* Return QEEP_OK if successful
*/
QEEP_RET pQrndRnd(pQrnd_Handle pQrnd, uint8_t *rnd, int32_t len);


/*
* Frees SDK Core Handle
* Return QEEP_OK if successful
*/
QEEP_RET pQrndClose(pQrnd_Handle pQrnd);


/*
* Entropy expansion
* Input parameters:
*        - factor: pQrnd Expansion parameter factor output random  =  factor times of random
*        - rnd: input random buffer, must be 16 dividable
*        - rnd_len: input random buffer length
*        - rnd_out: random output buffer, the length must be rnd_len * factor
* Return QEEP_OK if successful
*/
QEEP_RET pQrndExpansion(int32_t factor, uint8_t *rnd, int32_t rnd_len, uint8_t *rnd_out);


# ifdef  __cplusplus
}
# endif

#endif  /*_QISPACE_SDK_PQRND_H */