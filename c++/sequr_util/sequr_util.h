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

#define AES_KEY_HEADER_SIZE  0
#define QEEP_KEY_HEADER_SIZE  22
#define QEEP_PASS_HEADER_SIZE 24

 typedef enum _sequr_key_type {
    AES_KEY = 0,
    QEEP_KEY = 1,
    QEEP_PASS =2,
} sequr_key_type;

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
* Generate a key (ex. for AES or QEEP) on QiSpace Enterprise with provided key length
* Input parameters:
*       - sequr_handle: SequrHandle pointer
*       - key_size: keysize in bytes
*       - key_type: 0 for AES Key, 1 for QEEP Key, 2 for QEEP PASS
* Output parameters:
*       - key_id: pointer to key_id string
*       - key: pointer to the key buffer, which should be allocated before calling this function
               key_buffer_size >= key_size + Key_type_header_size if key_type is QEEP. 
* Return:
*       - key size on success, -1 on failure
*/
int sequr_util_key_gen(SequrHandle* sequr_handle, int32_t key_size, std::string& key_id, uint8_t *key, int key_type);


/*
* Get key (geneated by sequr_util_key_gen) from QiSpace Enterprise with provided key id
* Input parameters:
*       - sequr_handle: SequrHandle pointer
*       - key_id: key_id string
*       - key_type: 0 for AES Key, 1 for QEEP Key, 2 for QEEP PASS
* Output parameters:
*       - key: pointer to the key buffer, which should be allocated before calling this function
            key_buffer_size >= key_size + Key_type_header_size if key_type is QEEP. 
* Return:
*       - key size on success, -1 on failure
*/
int sequr_util_query_key(SequrHandle* sequr_handle, std::string& key_id, uint8_t *key, int key_type);


/*
* Free SEQUR Handle
* Input parameters:
*       - sequr_handle: SequrHandle pointer
*/
void sequr_free(SequrHandle* sequr_handle);


/*
* Get Quantum Entropy(QE) from QiSpace Enterprise
* Input parameters:
*       - sequr_handle: SequrHandle pointer
*       - QE: pointer to the QE buffer of len bytes, which should be allocated before calling this function
*       - len: required length of QE 
* Return:
*       - length of QE bytes on success, -1 on failure
*/
int sequr_util_get_qe(SequrHandle* sequr_handle, uint8_t *QE, int len);

/**
  Return the buffer size required by giving key_size in byte and key_type
 */
int32_t sequr_util_key_buff_size(int32_t key_size,  sequr_key_type key_type );

#endif  /*_SEQUR_UTIL_H */