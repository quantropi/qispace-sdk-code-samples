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

#include "masq_kem.h"

// Redefine this as print_to_console() when usb_pcdc works
#define PRINTF(...)

#define SEED_LEN 32

typedef struct _rand_handle {
    int seed_length;
    uint8_t seed[SEED_LEN];
} RAND_HANDLE;

void PRINTF_buf_hex(char *title, unsigned char *buf, int len );
int32_t rand_cf(RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num);
int32_t rand_seed_cf(RAND_HANDLE *rand_handle, uint8_t *seed, int32_t seed_length);
int kem_demo(void);


unsigned char seed_orig[32] =
    {0xC8,0x0E,0x6B,0xE7,0xBA,0xD6,0xA9,0xE0,0xF4,0x84,0x49,0x03,0x63,0xA1,0xB1,0xA6,
     0x21,0xAA,0xFE,0x87,0x3E,0x9F,0xE6,0x15,0xA3,0x95,0x9E,0x95,0xBB,0x13,0xB6,0xF5};

void PRINTF_buf_hex(char *title, unsigned char *buf, int len ) {
    int i;

    PRINTF("\n\r%s(%d) : ", title, len);
    for (i =0; i < 16; i++) {
     PRINTF("%02X", (buf[i]&0xff));
    }
    PRINTF("\n\r");
}


int32_t rand_cf(RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num)
{
    int i;

    if (rand_num == NULL) return 0;

    if (rand_handle == NULL)
    {
        for (i = 0; i < rand_length; i++)
        {
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
    } else {
        for (i = 0; i < rand_length; i++)
        {
            srand(rand_handle->seed[i%rand_handle->seed_length]);
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
    }
    return 1;
}

int32_t rand_seed_cf(RAND_HANDLE *rand_handle, uint8_t *seed, int32_t seed_length)
{
    if (seed == NULL) return 0;

    if(rand_handle == NULL){
       seed = seed_orig;
    } else {
        memcpy(rand_handle->seed, seed_orig, seed_length);
        rand_handle->seed_length = seed_length;
    }

    return 1;
}

int kem_demo(void)
{
    MASQ_API ret0 = 1, ret1 = 1, ret2 = 1, ret3 = 1;
    int ret4=1;
    MASQ_KEM_handle *kem_handle;
    int32_t len1, len2, len3, len4;
    unsigned char *pk=NULL, *sk=NULL, *ct=NULL, *ss_a=NULL, *ss_b=NULL;

    RAND_HANDLE rand_handle;

    kem_handle = MASQ_KEM_init(LEVEL5, rand_cf, rand_seed_cf, (struct MASQ_RAND_HANDLE_ *)&rand_handle);
    if (kem_handle == NULL) {
        goto bail;
    }

    ret0 = MASQ_KEM_seed(kem_handle, seed_orig, SEED_LEN);
    if(ret0 != MASQ_SUCCESS) {
        goto bail;
    }

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

    ret1 = MASQ_KEM_keypair(kem_handle, pk, sk);
    if(ret1 != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Keypair generation failed with error %d\n\r", ret1);
        goto bail;
    } else {
        PRINTF("MASQ KEM Keypair generated\n\r");
    }

    PRINTF_buf_hex("pk", pk, len1);
    PRINTF_buf_hex("sk", sk, len2);

    ret2 = MASQ_KEM_encaps(kem_handle, pk, ss_a, ct);
    if(ret2 != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Encapsulation failed with error %d\n\r", ret2);
        goto bail;
    } else {
        PRINTF("MASQ KEM Encapsulation succeed.\n\r");
    }

    PRINTF_buf_hex("ct", ct, len3);
    PRINTF_buf_hex("ss_a", ss_a, len4);

    ret3 = MASQ_KEM_decaps(kem_handle, sk, ct, ss_b);
    if(ret3 != MASQ_SUCCESS) {
        PRINTF("MASQ KEM Decapsulation failed with error %d\n\r", ret3);
    }

    PRINTF_buf_hex("ss_b", ss_b, len4);

    if (memcmp(ss_a, ss_b, (unsigned int)len4) != 0) {
        PRINTF("%s\n\r", "Error: Shared Secret do not match");
        ret4 = 0;
    }
    else {
        PRINTF("%s\n\r", "Success: Shared Secret Matches\n\r\n\r");
        ret4 = 1;
    }

bail:
    MASQ_KEM_free(kem_handle);
    if (pk != NULL) free(pk);
    if (sk != NULL) free(sk);
    if (ct != NULL) free(ct);
    if (ss_a != NULL) free(ss_a);
    if (ss_b != NULL) free(ss_b);

    if (ret0 == MASQ_SUCCESS && ret1==MASQ_SUCCESS && ret2==MASQ_SUCCESS && ret3==MASQ_SUCCESS && ret4 == 0) {
        return 0;
    } else {
        return 1;
    }
}
