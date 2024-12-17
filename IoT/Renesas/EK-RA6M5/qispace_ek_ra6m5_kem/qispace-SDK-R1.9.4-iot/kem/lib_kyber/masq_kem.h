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

#include <stdint.h>
#include "masq_kem_error.h"


typedef struct MASQ_KEM_HANDLE_   *MASQ_KEM_handle;

typedef struct MASQ_RAND_HANDLE_   *MASQ_RAND_handle;

/** MASQ_KEM_algs: Key Exchange algorithms' security levels */
typedef enum 
{
    KEM_LEVEL1,
    KEM_LEVEL3,
    KEM_LEVEL5
} MASQ_KEM_LEVEL;


/**
 * Callback function type to generate random numbers. 
 * Caller should always seed random generator and allocate enough memory.
 *
 * @param[in] rand_handle is the handle for random number generator.
 * @param[in] rand_length is the length of random required in bytes.
 * @param[out] rand is the generated random represented as a byte string.
 * @return The number of random generated in bytes, the value should equal rand_length if success. 
 *         Otherwise, return -1 if fail
*/
typedef int32_t (*MASQ_rand_callback_t)( MASQ_RAND_handle rand_handle, int32_t rand_length,  uint8_t *rand );



/**
 * Callback function type to seed random generator. 
 *
 * @param[in] rand_handle is the handle for random generator.
 * @param[in] seed represented as a byte string.
 * @param[in] seed_length is the length of seed required in bytes.
 * @return 0 if success Otherwise, return -1 if fail
*/
typedef int32_t (*MASQ_rand_seed_callback_t)( MASQ_RAND_handle rand_handle, uint8_t *seed,  int32_t seed_length );


/**
 * Constructs an MASQ_KEM handle for a particular key exchange algorithm using user provided random callback functions.
 *
 * Callers should always check whether the return value is `NULL`, which indicates either than an
 * invalid algorithm name was provided, or that the requested algorithm was disabled at compile-time.
 *
 * @param[in] level level is the security level defined in MASQ_KEM_LEVEL. KEM_LEVEL1 is the lowest security. KEM_LEVEL5 is the highest security.
 * @param[in] rand_cf is the the callback function to generate random using the generator that caller provides. 
 * @param[in] rand_seed_cf is the the callback function to seed the random generator that caller provides.
 * @param[in] rand_handle is the handle for the random generator that caller provides if rand_cf and rand_seed_cf requires. Could be "NULL" if does not require.
 * @return An MASQ_KEM_handle for the particular algorithm. 
 *         If fail, returns `NULL` if the algorithm has been disabled at compile-time.
 *         or returns `NULL` if rand_cf or rand_seed_cf are NULL.  
 */
MASQ_KEM_handle *MASQ_KEM_init( MASQ_KEM_LEVEL level, MASQ_rand_callback_t rand_cf, MASQ_rand_seed_callback_t rand_seed_cf, MASQ_RAND_handle rand_handle );


/**
 * Constructs an MASQ_KEM handle for a particular key exchnage algorithm using QiSpace NGen random callback functions.
 *
 * Callers should always check whether the return value is `NULL`, which indicates either than an
 * invalid algorithm name was provided, or the requested algorithm was disabled at compile-time.
 *
 * @param[in] level level is the security level defined in MASQ_KEM_LEVEL. KEM_LEVEL1 is the lowest security. KEM_LEVEL5 is the highest security.
 * @return An MASQ_KEM_handle for the particular algorithm, or `NULL` if the algorithm has been disabled at compile-time.
 */
MASQ_KEM_handle *MASQ_KEM_qeep_init( MASQ_KEM_LEVEL level );

/**
 * Seed random status for MASQ operations.
 *
 * @param[in] kem is the MASQ_KEM handle representing the KEM.
 * @param[in] seed is the random seed represented as a byte string. Caller is responsible to generate a good quality of seed string.
 * @param[in] seed_length is the seed length in bytes.
 * @return MASQ_KEM_SUCCESS or MASQ_KEM_ERROR
 */
MASQ_KEM_API MASQ_KEM_seed( MASQ_KEM_handle *kem, uint8_t *seed, int32_t seed_length );


/**
* Public key length based on MASQ_KEM algorithm.
* @param[in] kem is the MASQ_KEM handle representing the KEM.
* @return the length of the public key.
*/
int32_t MASQ_KEM_public_key_length( MASQ_KEM_handle *kem );


/**
* Secret key length based on MASQ_KEM algorithm.
* @param[in] kem is the MASQ_KEM handle representing the KEM.
* @return the length of the secret key. 
*/
int32_t MASQ_KEM_secret_key_length( MASQ_KEM_handle *kem );


/**
* Ciphertext length based on MASQ_KEM algorithm.
* @param[in] kem is the MASQ_KEM handle representing the KEM.
* @return the length of the ciphertext. 
*/
int32_t MASQ_KEM_ciphertext_length( MASQ_KEM_handle *kem );


/**
* Shared secret length based on MASQ_KEM algorithm.
* @param[in] is the The MASQ_KEM handle representing the KEM.
* @return the length of the shared secret.
*/
int32_t MASQ_KEM_shared_secret_length( MASQ_KEM_handle *kem );


/**
 * Keypair generation API.
 *
 * Caller is responsible for allocating sufficient memory for `public_key` and
 * `secret_key`,  could be obtained by calling the helper functions.
 *
 * @param[in] kem is the MASQ_KEM handle representing the KEM.
 * @param[out] public_key is the public key represented as a byte string.
 * @param[out] secret_key is the secret key represented as a byte string.
 * @return MASQ_KEM_SUCCESS or MASQ_KEM_ERROR
 */
MASQ_KEM_API MASQ_KEM_keypair( MASQ_KEM_handle *kem, uint8_t *public_key, uint8_t *secret_key );


/**
 * Encapsulation API.
 *
 * Caller is responsible for allocating sufficient memory for `ciphertext` and
 * `shared_secret`, could be obtained by calling the helper functions.
 *
 * @param[in] kem is the MASQ_KEM handle representing the KEM.
 * @param[in] public_key is the public key represented as a byte string.
 * @param[out] shared_secret is the shared secret represented as a byte string. 
 * @param[out] ciphertext is the ciphertext (encapsulation) represented as a byte string.
 * @return MASQ_KEM_SUCCESS or MASQ_KEM_ERROR
 */
MASQ_KEM_API MASQ_KEM_encaps( MASQ_KEM_handle *kem, uint8_t *public_key, uint8_t *shared_secret, uint8_t *ciphertext );


/**
 * Decapsulation API.
 *
 * Caller is responsible for allocating sufficient memory for `shared_secret`, 
 * Can be obtained by calling the helper functions. 
 *
 * @param[in] kem is the MASQ_KEM object representing the KEM.
 * @param[in] secret_key is the secret key represented as a byte string.
 * @param[in] ciphertext is the ciphertext (encapsulation) represented as a byte string.
 * @param[out] shared_secret is the shared secret represented as a byte string.
 * @return MASQ_KEM_SUCCESS or MASQ_KEM_ERROR
 */
MASQ_KEM_API MASQ_KEM_decaps( MASQ_KEM_handle *kem, uint8_t *secret_key, uint8_t *ciphertext, uint8_t *shared_secret );


/**
 * Frees an MASQ_KEM_handle that was constructed by MASQ_KEM_init.
 *
 * @param[in] kem is the MASQ_KEM handle to free.
 */
MASQ_KEM_API MASQ_KEM_free( MASQ_KEM_handle *kem );


/**
 * Frees an MASQ_KEM_handle that was constructed by MASQ_KEM_qeep_init.
 *
 * @param[in] kem is the MASQ_KEM handle to free.
 */
MASQ_KEM_API MASQ_KEM_qeep_free( MASQ_KEM_handle *kem );

/**
 * Call this API if want to use MASQ_KEM_KEY_VERSION key format from Release 1.9.4 only
 *
 * @param[in] kem is the MASQ_KEM handle representing the KEM.
 * @return MASQ_KEM_SUCCESS or MASQ_ERR_xxx
 */
MASQ_KEM_API MASQ_KEM_use_key_format( MASQ_KEM_handle *kem);

# ifdef  __cplusplus
}
# endif

#endif /*_QISPACE_MASQ_KEM_H*/