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


#ifndef _QISPACE_MASQ_KEM_H
#define _QISPACE_MASQ_KEM_H

# ifdef  __cplusplus
extern "C" {
# endif

/* MASQ_KEM_algs: Key Exchange algorithms' security levels */
typedef enum 
{
    KEM_LEVEL1,
    KEM_LEVEL3,
    KEM_LEVEL5
} MASQ_KEM_LEVEL;

typedef struct MASQ_KEM_HANDLE_   *MASQ_KEM_handle;

typedef struct MASQ_RAND_HANDLE_  *MASQ_RAND_handle;

#if defined(MASQ_KEM_hppk)

#define MASQ_KEM_init masq_kem_hppk_init
#define MASQ_KEM_qeep_init masq_kem_hppk_qeep_init
#define MASQ_KEM_seed masq_kem_hppk_seed
#define MASQ_KEM_public_key_length masq_kem_hppk_public_key_length
#define MASQ_KEM_secret_key_length masq_kem_hppk_secret_key_length
#define MASQ_KEM_ciphertext_length masq_kem_hppk_ciphertext_length
#define MASQ_KEM_shared_secret_length masq_kem_hppk_shared_secret_length
#define MASQ_KEM_keypair masq_kem_hppk_keypair
#define MASQ_KEM_encaps masq_kem_hppk_encaps
#define MASQ_KEM_decaps masq_kem_hppk_decaps
#define MASQ_KEM_free masq_kem_hppk_free
#define MASQ_KEM_qeep_free masq_kem_hppk_qeep_free
#define MASQ_KEM_use_key_format masq_kem_hppk_use_key_format


#include "masq_kem_hppk.h"

#else

//If user do not specify ALG, default is ML-KEM
#if !defined(MASQ_KEM_mlkem)
#pragma message("MASQ_KEM_mlkem is not defined. We will define it.")
#define MASQ_KEM_mlkem
#endif

#define MASQ_KEM_init masq_kem_mlkem_init
#define MASQ_KEM_qeep_init masq_kem_mlkem_qeep_init
#define MASQ_KEM_seed masq_kem_mlkem_seed
#define MASQ_KEM_public_key_length masq_kem_mlkem_public_key_length
#define MASQ_KEM_secret_key_length masq_kem_mlkem_secret_key_length
#define MASQ_KEM_ciphertext_length masq_kem_mlkem_ciphertext_length
#define MASQ_KEM_shared_secret_length masq_kem_mlkem_shared_secret_length
#define MASQ_KEM_keypair masq_kem_mlkem_keypair
#define MASQ_KEM_encaps masq_kem_mlkem_encaps
#define MASQ_KEM_decaps masq_kem_mlkem_decaps
#define MASQ_KEM_free masq_kem_mlkem_free
#define MASQ_KEM_qeep_free masq_kem_mlkem_qeep_free
#define MASQ_KEM_keypair_derand masq_kem_mlkem_keypair_derand
#define MASQ_KEM_encaps_derand masq_kem_mlkem_encaps_derand

#include "masq_kem_mlkem.h"

#endif

# ifdef  __cplusplus
}
# endif

#endif /*_QISPACE_MASQ_KEM_H*/