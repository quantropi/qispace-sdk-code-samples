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
#ifndef _QISPACE_MASQ_DS_MLDSA_H
#define _QISPACE_MASQ_DS_MLDSA_H

# ifdef  __cplusplus
extern "C" {
# endif

#include <stdint.h>
#include "masq_ds_error.h"

#define RND_BYTES 32

/**
 * Callback function to generate random numbers. 
 * Caller should always seed rand generator and allocate enough memory.
 *
 * @param[in] rand_handle is the handle for random number generator.
 * @param[in] rand_length is the length of random required in bytes.
 * @param[out] rand is the generated random represented as a byte string.
 * @return the number of random generated in bytes, the value should equal rand_length if success. 
 *         -1 if fail.
*/
typedef int32_t (*MASQ_rand_callback_t)( MASQ_RAND_handle rand_handle, int32_t rand_length, uint8_t *rand );


/**
 * Callback function type to seed random generator. 
 *
 * @param[in] rand_handle is the handle for random generator.
 * @param[in] seed is the seed represented as a byte string.
 * @param[in] seed_length is the length of seed required in bytes.
 * @return 0 if success, -1 if fail.
*/
typedef int32_t (*MASQ_rand_seed_callback_t)( MASQ_RAND_handle rand_handle, uint8_t *seed, int32_t seed_length );


/**
 * Constructs a MASQ_DS handle for a particular Digital Signature algorithm using user provided random callback functions.
 *
 * Callers should always check whether the return value is `NULL`, which indicates either an
 * invalid algorithm name was provided, or the requested algorithm was disabled at compile-time.
 *
 * @param[in] level is the security level defined in MASQ_DS_LEVEL. DS_LEVEL1 is the lowest security. DS_LEVEL5 is the highest security.
 * @param[in] rand_cf is the callback function to generate random using the generator that caller provides. 
 * @param[in] rand_seed_cf is the callback function to seed the random generator that caller provides.
 * @param[in] rand_handle is the handle for the random generator that caller provides if rand_cf and rand_seed_cf requires. Could be "NULL" if does not require.
 * @return a MASQ_DS_handle for the particular algorithm.
 *         If fail, returns `NULL` if the algorithm has been disabled at compile-time,
 *         or returns `NULL` if rand_cf or rand_seed_cf are NULL.  
 */
MASQ_DS_handle *masq_ds_mldsa_init( MASQ_DS_LEVEL level, MASQ_rand_callback_t rand_cf, MASQ_rand_seed_callback_t rand_seed_cf, MASQ_RAND_handle rand_handle );


/**
 * Constructs a MASQ_DS handle for a particular Digital Signature algorithm using QiSpace NGen pQrnd based random callback functions.
 *
 * Callers should always check whether the return value is `NULL`, which indicates either an
 * invalid algorithm name was provided, or the requested algorithm was disabled at compile-time.
 *
 * @param[in] level is the security level defined in MASQ_DS_LEVEL. DS_LEVEL1 is the lowest security. DS_LEVEL5 is the highest security.
 * @return a MASQ_DS_handle for the particular algorithm, or `NULL` if the algorithm has been disabled at compile-time.
 */
MASQ_DS_handle *masq_ds_mldsa_qeep_init( MASQ_DS_LEVEL level );


/**
 * Seed random status for MASQ operations.
 *
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[in] seed is the random seed represented as a byte string. Caller is responsible to generate a good quality of seed string.
 * @param[in] seed_length is the seed length in bytes.
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_seed( MASQ_DS_handle *ds, uint8_t *seed, int32_t seed_length );


/**
* Public key length based on MASQ_DS algorithm.
* @param[in] ds is the MASQ_DS handle representing the DS.
* @return the length of the public key. 
*/
int32_t masq_ds_mldsa_public_key_length( MASQ_DS_handle *ds );


/**
* Private key length based on MASQ_DS algorithm.
* @param[in] ds is the MASQ_DS handle representing the DS.
* @return the length of the private key. 
*/
int32_t masq_ds_mldsa_private_key_length( MASQ_DS_handle *ds );


/**
* Signature length based on MASQ_DS algorithm.
* @param[in] ds is the MASQ_DS handle representing the DS.
* @return the length of the ciphertext. 
*/
int32_t masq_ds_mldsa_signature_length( MASQ_DS_handle *ds );


/**
 * Keypair generation API.
 *
 * Caller is responsible for allocating sufficient memory for `public_key` and
 * `private_key`,  could be obtained by calling the helper functions.
 *
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[out] public_key is the public key represented as a byte string.
 * @param[out] private_key is the secret key represented as a byte string.
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_keypair( MASQ_DS_handle *ds, uint8_t *public_key, uint8_t *private_key );


/**
 * Sign API.
 *
 * Caller is responsible for allocating sufficient memory for `signature` and
 * `signature_len`, could be obtained by calling the helper functions.
 *
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[in] private_key is the private key represented as a byte string.
 * @param[in] message is the message represented as a byte string.
 * @param[in] message_len is the length of the message to sign.
 * @param[in] ctx is a pointer to context string
 * @param[in] ctxlen is the length of context string
 * @param[out] signature is the signature on the message represented as a byte string.
 * @param[out] signature_len is the length of the signature.
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_sign( MASQ_DS_handle *ds, uint8_t *private_key, uint8_t *message, int32_t message_len, uint8_t *signature, int32_t *signature_len, uint8_t* ctx, size_t ctxlength );


/**
 * Verify API.
 *
 * Caller is responsible for allocating sufficient memory for `public_key` and
 * `signature`, could be obtained by calling the helper functions.
 * 
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[in] public_key is the public key represented as a byte string.
 * @param[in] message is the message represented as a byte string.
 * @param[in] message_len is the length of the message.
 * @param[in] signature is the signature on the message represented as a byte string.
 * @param[in] signature_len is the length of the signature.
 * @param[in] ctx is a pointer to context string
 * @param[in] ctxlen is the length of context string
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_verify( MASQ_DS_handle *ds, uint8_t *public_key, uint8_t *message, int32_t message_len, uint8_t *signature, int32_t signature_len, uint8_t* ctx, size_t ctxlength );


/**
 * Frees MASQ_DS_handle that was constructed by MASQ_DS_init.
 *
 * @param[in] ds is the MASQ_DS handle to free.
 */
MASQ_DS_API masq_ds_mldsa_free( MASQ_DS_handle *ds );


/**
 * Frees MASQ_DS_handle that was constructed by MASQ_DS_qeep_init.
 *
 * @param[in] ds is the MASQ_DS handle to free.
 */
MASQ_DS_API masq_ds_mldsa_qeep_free(MASQ_DS_handle *ds);

/**
 * Sign API. Internal API.
 *
 * Caller is responsible for allocating sufficient memory for `signature` and
 * `signature_len`, could be obtained by calling the helper functions.
 *
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[in] private_key is the private key represented as a byte string.
 * @param[in] message is the message represented as a byte string.
 * @param[in] message_len is the length of the message to sign.
 * @param[in] ctx is a pointer to context string
 * @param[in] ctxlen is the length of context string
 * @param[out] signature is the signature on the message represented as a byte string.
 * @param[out] signature_len is the length of the signature.
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_signature_internal( MASQ_DS_handle *ds, uint8_t *private_key, uint8_t *message, int32_t message_len, uint8_t *signature, int32_t *signature_len, uint8_t* ctx, size_t ctxlength, const uint8_t rnd[RND_BYTES] );


/**
 * Verify API. Internal API.
 *
 * Caller is responsible for allocating sufficient memory for `public_key` and
 * `signature`, could be obtained by calling the helper functions.
 * 
 * @param[in] ds is the MASQ_DS handle representing the DS.
 * @param[in] public_key is the public key represented as a byte string.
 * @param[in] message is the message represented as a byte string.
 * @param[in] message_len is the length of the message.
 * @param[in] signature is the signature on the message represented as a byte string.
 * @param[in] signature_len is the length of the signature.
 * @param[in] ctx is a pointer to context string
 * @param[in] ctxlen is the length of context string
 * @return MASQ_DS_SUCCESS or MASQ_DS_ERROR_*
 */
MASQ_DS_API masq_ds_mldsa_verify_internal( MASQ_DS_handle *ds, uint8_t *public_key, uint8_t *message, int32_t message_len, uint8_t *signature, int32_t signature_len, uint8_t* ctx, size_t ctxlength );


# ifdef  __cplusplus
}
# endif

#endif /*_QISPACE_MASQ_DS_MLDSA_H*/