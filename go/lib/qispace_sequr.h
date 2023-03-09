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

#ifndef _QISPACE_SDK_SEQUR_H
#define _QISPACE_SDK_SEQUR_H

# ifdef  __cplusplus
extern "C" {
# endif

#include <stdint.h>
#include "error.h"

/* QiSpace SDK SEQUR APIs */

typedef struct QISPACE_SDK_Core_Handle_   *QSC_Handle;

/*
    Initialize SDK Core Handle
    Return QEEP_OK if successful
*/

QEEP_RET QSC_init(QSC_Handle *pQSC);

/*
    Load the QEEP key to encode or decode, which is retrieved from QiSpace platform
    Input parameters:
        - QSC: qsc_handle created by QSC_init
        - qk: from QiSpace platform
        - qk_len: qk buffer length in bytes
    Return QEEP_OK if successful
*/

QEEP_RET QSC_qeep_key_load(QSC_Handle QSC, uint8_t *qk, int32_t qk_len);

/*
    Load IV to encode or decode, which is a 16 byte random number
    Input parameters:
        - QSC: qsc_handle created by QSC_init
        - iv: 16 byte random number
        - iv_len: iv buffer length in bytes, must be 16
    Return QEEP_OK if successful
*/

QEEP_RET QSC_qeep_iv_set(QSC_Handle QSC, uint8_t *iv, int32_t iv_len);

/*
    QEEP decode can be used for an encoded message of any length by calling this API multiple times
    Divide the message into sections (multiple of 16), the last section can be any length (less than 16)
    Input parameters:
        - QSC: qsc_handle created by QSC_init
        - encoded_msg: input encoded message for encoding
        - encoded_msg_len: message length in bytes
        - msg: the decoded message
    Return QEEP_OK if successful
*/

QEEP_RET QSC_qeep_decode(QSC_Handle QSC, uint8_t *encoded_msg, int32_t encoded_msg_len, uint8_t *msg);

/*
    Frees SDK Core Handle
    Return QEEP_OK if successful
*/

QEEP_RET QSC_close(QSC_Handle QSC);

# ifdef  __cplusplus
}
# endif

#endif  /*_QISPACE_SDK_SEQUR_H */