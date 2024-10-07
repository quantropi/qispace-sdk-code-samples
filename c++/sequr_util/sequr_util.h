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

#include "qispace_qeep.h"

/* QiSpace SEQUR QK APIs */
typedef struct {
    std::string url;   // QiSpace Enterprise URL
    std::string token; // device token
} qispace_meta;


typedef struct {
    qispace_meta* q_meta; // Pointer to QiSpace metadata (URL and token)
    QP_Handle qp_handle; // Pointer to the QP_Handle
} SequrHandle;


/*
* Initialize SEQUR Handle
* Input parameters: 
*   - q_meta: JSON string containing QiSpace Enterprise URL and device token
* Return:
*   - pointer to newly allocated SequrHandle or nullptr if allocation fails
*/
SequrHandle* sequr_util_init(const std::string q_meta);


/*
* Generate QK on QiSpace Enterprise with provided key length
* Input parameters:
*       - SequrHandle: SequrHandle pointer
*       - keysize: keysize in bytes
* Output parameters:
*       - key_id: pointer to key_id string
*       - key: pointer to the key buffer, which should be allocated before calling this function
* Return:
*       - keysize on success, -1 on failure
*/
int sequr_util_key_gen(SequrHandle* sequrHandle, int32_t keysize, std::string& key_id, uint8_t *key);


/*
* Get QK from QiSpace Enterprise with provided key id
* Input parameters:
*       - SequrHandle: sequrHandle pointer
*       - key_id: key_id string
* Output parameters:
*       - key: pointer to the key buffer, which should be allocated before calling this function
* Return:
*       - keysize on success, -1 on failure
*/
int sequr_util_query_key(SequrHandle* sequrHandle, std::string& key_id, uint8_t *key);

/*
* Free SEQUR Handle
* Input parameters:
*       - sequr_handle: SequrHandle pointer
*/
void sequr_free(SequrHandle* sequr_handle);

#endif  /*_SEQUR_UTIL_H */