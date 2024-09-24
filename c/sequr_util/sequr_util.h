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

#ifndef _SEQUR_UTIL_H
#define _SEQUR_UTIL_H

# ifdef  __cplusplus
extern "C" {
# endif

#include <stdint.h>
#include "qispace_qeep.h"



/* QiSpace SEQUR QK APIs */
typedef struct {
    char *url;   // QiSpace Enterprise URL
    char *token; // device token
} qispace_meta;

typedef struct {
    qispace_meta *q_meta;  // Pointer to QiSpace metadata (URL and token)
    QP_Handle qp_handle; // Pointer to the QP_Handle
} sequr_handle;


/*
* Initialize SEQUR Handle
* Input parameters: 
*   - q_meta: JSON string containing QiSpace Enterprise URL and device token
* Return pointer to newly allocated sequr_handle or NULL if allocation fails
*/
sequr_handle* sequr_util_init(char* q_meta);


/*
* Generate QK on QiSpace Enterprise with provided key length
* Input parameters:
*       - sequr_handle: sequr_handle pointer
*       - keysize: keysize in bytes
* Output paramerers:
*       - key_id: pointer to key_id string
*       - key: pointer to the key buffer allocated before calling
* Return keysize on success, -1 on failure
*/
int sequr_util_key_gen(sequr_handle* sequr_handle, int32_t keysize_bytes, char* key_id, uint8_t *key);


/*
* Get QK from QiSpace Enterprise with provided key id
* Input parameters:
*       - sequr_handle: sequr_handle pointer
*       - key_id: key_id string
* Output paramerers:
*       - key: pointer to the key buffer allocated before calling
* Return keysize on success, -1 on failure
*/
int sequr_util_query_key(sequr_handle* sequr_handle, char* key_id, uint8_t *key);

/*
* free SEQUR Handle
* Input parameters:
*       - sequr_handle: sequr_handle pointer
*/
void sequr_free(sequr_handle* sequr_handle);

# ifdef  __cplusplus
}
# endif

#endif  /*_SEQUR_UTIL_H */