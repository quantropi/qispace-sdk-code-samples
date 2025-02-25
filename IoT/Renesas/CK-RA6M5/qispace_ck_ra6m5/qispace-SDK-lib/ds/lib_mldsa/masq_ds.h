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
#ifndef _QISPACE_MASQ_DS_H
#define _QISPACE_MASQ_DS_H

# ifdef  __cplusplus
extern "C" {
# endif

/* MASQ_DS_algs: Digital Signature algorithms' security levels */
typedef enum 
{
    DS_LEVEL1,
    DS_LEVEL3,
    DS_LEVEL5
} MASQ_DS_LEVEL;

typedef struct MASQ_DS_HANDLE_    *MASQ_DS_handle;

typedef struct MASQ_RAND_HANDLE_  *MASQ_RAND_handle;

#if defined(MASQ_DS_ghppk)

#define MASQ_DS_init masq_ds_ghppk_init
#define MASQ_DS_qeep_init masq_ds_ghppk_qeep_init
#define MASQ_DS_seed masq_ds_ghppk_seed
#define MASQ_DS_public_key_length masq_ds_ghppk_public_key_length
#define MASQ_DS_private_key_length masq_ds_ghppk_private_key_length
#define MASQ_DS_signature_length masq_ds_ghppk_signature_length
#define MASQ_DS_keypair masq_ds_ghppk_keypair
#define MASQ_DS_free masq_ds_ghppk_free
#define MASQ_DS_qeep_free masq_ds_ghppk_qeep_free
#define MASQ_DS_use_key_format masq_ds_ghppk_use_key_format
#define MASQ_DS_sign(handle,sk,msg,msg_len,signature,signature_len,ctx,ctx_length) masq_ds_ghppk_sign(handle,sk,msg,msg_len,signature,signature_len)
#define MASQ_DS_verify(handle,pk,msg,msg_len,signature,signature_len,ctx,ctx_length) masq_ds_ghppk_verify(handle,pk,msg,msg_len,signature,signature_len)

#include "masq_ds_ghppk.h"

#else

//If user do not specify ALG, default is mldsa
#if !defined(MASQ_DS_mldsa)
#pragma message("MASQ_DS_mldsa is not defined. We will define it.")
#define MASQ_DS_mldsa
#endif

#define MASQ_DS_init masq_ds_mldsa_init
#define MASQ_DS_qeep_init masq_ds_mldsa_qeep_init
#define MASQ_DS_seed masq_ds_mldsa_seed
#define MASQ_DS_public_key_length masq_ds_mldsa_public_key_length
#define MASQ_DS_private_key_length masq_ds_mldsa_private_key_length
#define MASQ_DS_signature_length masq_ds_mldsa_signature_length
#define MASQ_DS_keypair masq_ds_mldsa_keypair
#define MASQ_DS_verify masq_ds_mldsa_verify
#define MASQ_DS_sign masq_ds_mldsa_sign
#define MASQ_DS_free masq_ds_mldsa_free
#define MASQ_DS_qeep_free masq_ds_mldsa_qeep_free
#define MASQ_DS_verify_internal masq_ds_mldsa_verify_internal
#define MASQ_DS_signature_internal masq_ds_mldsa_signature_internal

#include "masq_ds_mldsa.h"

#endif

# ifdef  __cplusplus
}
# endif

#endif /*_QISPACE_MASQ_DS_H*/