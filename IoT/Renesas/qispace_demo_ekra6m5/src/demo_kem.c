/******************************************************************************
 *
 * Quantropi CONFIDENTIAL
 * ----------------------
 *
 * This file is proprietary code and subject to the terms and conditions
 * defined in the file 'LICENSE', which is part of this source code package.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Quantropi Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Quantropi Incorporated and its suppliers and
 * may be covered by Canadian, U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Quantropi Incorporated.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "masq_kem.h"

int Q_printf(const char *format, ...);
#define PRINTF Q_printf

#define SEED_LEN 500
unsigned char seed_orig[SEED_LEN];

// In this demo, we use this structure to simply hold the random seed
// User may define more complicated date structure to have different way to generate random number
typedef struct _rand_handle {
    int seed_length;
    uint8_t seed[SEED_LEN];
    int idx;
} RAND_HANDLE;

void PRINTF_buf_hex(char *title, unsigned char *buf, int len );
void seed_init(void);
int32_t rand_cf(RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num);
int32_t rand_seed_cf(RAND_HANDLE *rand_handle, uint8_t *seed, int32_t seed_length);
void kem_demo(void);

void PRINTF_buf_hex(char *title, unsigned char *buf, int len ) {
    int i;
    int max_len = 16;

    if (len<16) {
        max_len = len;
    }

    PRINTF("\n\r%s(%d) : ", title, len);
    for (i =0; i < max_len; i++) {
     PRINTF("%02X", (buf[i]&0xff));
    }
    if (len > max_len) {
        PRINTF("......\n\r");
    } else {
        PRINTF("\n\r");
    }
}

// generate random seed
void seed_init(void)
{
    srand((unsigned int)time(0));
    for(int i = 0; i < SEED_LEN; i++){
        seed_orig[i] = (unsigned char)rand() & 0xff;
    }
}

unsigned char *kem_rseed;
// The random callback function to be called by MASQ_KEM_keypair() and MASQ_KEM_encaps()
int32_t rand_cf(RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num)
{
    int i;

    if (rand_num == NULL) return 0;

    if (rand_handle == NULL)
    {
        // If no rand data structure used, no good way to pass infomation from rand_seed_cf to rand_cf
        for (i = 0; i < rand_length; i++)
        {
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
    } else {
        // The structure RAND_HANDLE is used to hold the random seed
        if (rand_handle->idx > rand_handle->seed_length) {
            rand_handle->idx = 0;
        }
        for (i = 0; i < rand_length; i++)
        {
            srand(rand_handle->seed[(rand_handle->idx+i) % rand_handle->seed_length]);
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
        rand_handle->idx += rand_length;
    }
//    PRINTF_buf_hex("random", rand_num, rand_length);
    return 1;
}

// random seed callback function to be called by MASQ_KEM_seed()
int32_t rand_seed_cf(RAND_HANDLE *rand_handle, uint8_t *seed, int32_t seed_length)
{
    int seedlen;

    if (seed == NULL) return 0;

    if(rand_handle == NULL){
        kem_rseed = seed;
    } else {
        if (seed_length > SEED_LEN) {
            seedlen = SEED_LEN;
        } else {
            seedlen = seed_length;
        }
        memcpy(rand_handle->seed, seed, (size_t)seedlen);
        rand_handle->seed_length = seedlen;
        rand_handle->idx = 0;
    }

    return 1;
}

void kem_demo(void)
{
    MASQ_API ret;
    MASQ_KEM_handle *kem_handle;
    int32_t len1, len2, len3, len4;
    unsigned char *pk=NULL, *sk=NULL, *ct=NULL, *ss_a=NULL, *ss_b=NULL;

    RAND_HANDLE rand_handle;    // hold the random seed for random callback function to use

    // Initialize KEM data structure
    // We are going to use NIST level 5, you can also change it to Level 3 or Level 1 with lower security
    // The random callback function rand_cf() will be called later by MASQ_KEM_keypair and MASQ_KEM_encaps
    // The random seed callback function rand_seed_cf() will be called later by MASQ_KEM_seed
    // The RAND_HANDLE and rand_handle will be used to pass information from rand_seed_cf to rand_cf
    // If rand_handle is NULL, you may pass the random seed by using global variable kem_rseed
    // Using rand_handle may give user a chance to generate random number with more possibility
    kem_handle = MASQ_KEM_init(LEVEL5,  (MASQ_rand_callback_t)rand_cf, (MASQ_rand_seed_callback_t)rand_seed_cf, (struct MASQ_RAND_HANDLE_ *)&rand_handle);
    if (kem_handle == NULL) {
        goto err;
    }

    // generate random seed
    seed_init();

    // Call rand_seed_cf() with random seed passed in
    ret = MASQ_KEM_seed(kem_handle, seed_orig, SEED_LEN);
    if(ret != MASQ_SUCCESS) {
        goto err;
    }

    // get the length of public key, secret key, cipher text and shared secret
    len1 = MASQ_KEM_public_key_length(kem_handle);
    len2 = MASQ_KEM_secret_key_length(kem_handle);
    len3 = MASQ_KEM_ciphertext_length(kem_handle);
    len4 = MASQ_KEM_shared_secret_length(kem_handle);

    PRINTF("pk_len: %d, sk_len:%d, cipher_len:%d, ss_len:%d \n\r", len1, len2, len3, len4 );

    pk = malloc((size_t)len1);
    sk = malloc((size_t)len2);
    ct = malloc((size_t)len3);
    ss_a = malloc((size_t)len4);
    ss_b = malloc((size_t)len4);

    // generate key pairs
    ret = MASQ_KEM_keypair(kem_handle, pk, sk);
    if(ret != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Keypair generation failed with error %d\n\r", ret);
        goto err;
    } else {
        PRINTF("MASQ KEM Keypair generated\n\r");
    }

    PRINTF_buf_hex("pk", pk, len1);
    PRINTF_buf_hex("sk", sk, len2);

    // Send your public key to your friend

    // Your friend use your public key to encapsulate secret (ss_a) he/she want to share with you
    // and get output of the cipher text
    ret = MASQ_KEM_encaps(kem_handle, pk, ss_a, ct);
    if(ret != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Encapsulation failed with error %d\n\r", ret);
        goto err;
    } else {
        PRINTF("MASQ KEM Encapsulation succeed.\n\r");
    }

    PRINTF_buf_hex("ct", ct, len3);
    PRINTF_buf_hex("ss_a", ss_a, len4);

    // You received the cipher text generated by your friend
    // then you use your secret key to decapsulate the cipher and get the secret your friend shared with you
    ret = MASQ_KEM_decaps(kem_handle, sk, ct, ss_b);
    if(ret != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Decapsulation failed with error %d\n\r", ret);
    }

    PRINTF_buf_hex("ss_b", ss_b, len4);

    // For demo purpose
    // Here to make sure that the shared secret you got is correct
    if (memcmp(ss_a, ss_b, (unsigned int)len4) != 0) {
        PRINTF("%s\n\r", "Error: Shared Secret do not match");
    }
    else {
        PRINTF("%s\n\r", "Success: Shared Secret Matches\n\r\n\r");
    }

err:
    MASQ_KEM_free(kem_handle);
    if (pk != NULL) free(pk);
    if (sk != NULL) free(sk);
    if (ct != NULL) free(ct);
    if (ss_a != NULL) free(ss_a);
    if (ss_b != NULL) free(ss_b);
}
