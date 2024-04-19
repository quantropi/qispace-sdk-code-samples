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

#ifndef _QISPACE_SDK_QEEP_H
#define _QISPACE_SDK_QEEP_H

# ifdef  __cplusplus
extern "C" {
# endif

#include <stdint.h>
#include "error.h"

/* QiSpace SDK QEEP APIs */

typedef struct QISPACE_SDK_Core_Handle_   *QP_Handle;



/*
* Initialize SDK Core Handle
* Return QEEP_OK if successful
*/
QEEP_RET QP_init(QP_Handle *pQP);



/*
* Load the QEEP key to encode or decode, which is retrieved from QiSpace platform
* Input parameters:
*        - QP: handle created by QP_init
*        - qk: from QiSpace platform
*        - qk_len: qk buffer length in bytes
* Return QEEP_OK if successful
*/
QEEP_RET QP_qeep_key_load(QP_Handle QP, uint8_t *qk, int32_t qk_len);
QEEP_RET QP_qeep_key_load_en(QP_Handle QP, uint8_t *qk, int32_t qk_len);
QEEP_RET QP_qeep_key_load_de(QP_Handle QP, uint8_t *qk, int32_t qk_len);



/*
* Load IV to encrypt or decrypt, which is a 16 bytes random number
* Input parameters:
*        - QP: QP_handle created by QP_init
*        - iv: 16 bytes random number
*        - iv_len: iv buffer length in bytes, must be 16
* Return QEEP_OK if successful
*/
QEEP_RET QP_iv_set(QP_Handle QP, uint8_t *iv, int32_t iv_len);



/*
* QEEP encrypt can be used for an input message of any length by calling this API multiple times.
* Divide the message into sections (multiple of 16), the last section can be any length (less than 16).
* Input parameters:
*        - QP: QP_handle created by QP_init
*        - plaintext: input message for encrypt
*        - len: plaintext length in bytes
*        - ciphertext: the encrypted message
* Return QEEP_OK if successful
*/
QEEP_RET QP_encrypt(QP_Handle QP, uint8_t *plaintext, int32_t len, uint8_t *ciphertext);



/*
* QEEP decrypt can be used for an ciphertext of any length by calling this API multiple times.
* Divide the message into sections (multiple of 16), the last section can be any length (less than 16).
* Input parameters:
*        - QP: QP_handle created by QP_init
*        - ciphertext: input encoded message for decrypt
*        - len: ciphertext length in bytes
*        - plaintext: the decrypted message
* Return QEEP_OK if successful
*/
QEEP_RET QP_decrypt(QP_Handle QP, uint8_t *ciphertext, int32_t len, uint8_t *plaintext);



/*
* Frees SDK Core Handle
* Return QEEP_OK if successful
*/
QEEP_RET QP_close(QP_Handle QP);



# ifdef  __cplusplus
}
# endif

#endif  /*_QISPACE_SDK_QEEP_H */