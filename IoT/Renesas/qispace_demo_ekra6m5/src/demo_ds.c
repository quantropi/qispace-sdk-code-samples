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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "masq_ds.h"
#include "masq_ds_error.h"

int Q_printf(const char *format, ...);
#define PRINTF Q_printf

#define DS_SEED_LEN 500
unsigned char seed_src[DS_SEED_LEN];

#define MSG_LEN 100

typedef struct _ds_rand_handle {
    int DS_SEED_LENgth;
    uint8_t seed[DS_SEED_LEN];
    int idx;
} DS_RAND_HANDLE;

void dump_hex(unsigned char *buf, int len );
void ds_seed_init(void);
int32_t ds_rand_cf(DS_RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num);
int32_t ds_rand_seed_cf(DS_RAND_HANDLE *rand_handle, uint8_t *seed, int32_t seed_length);
void ds_demo(void);


uint8_t msg[MSG_LEN] =
    {0xF1,0x25,0xEE,0x6A,0xB9,0x21,0x7F,0x07,0x91,0x52,0x65,0x7B,0x11,0xB3,0x58,0x21,
     0xA2,0xAA,0xAF,0xCF,0x22,0xCE,0x60,0x8A,0x6A,0xE2,0xA0,0x66,0xC6,0x22,0xAA,0xCA,
     0x76,0xDA,0x87,0xF2,0xF2,0x21,0x16,0xB3,0x5A,0xA2,0x28,0x21,0xE7,0x6F,0xB7,0xD1,
     0xA0,0xA2,0x0C,0x0F,0x6F,0x42,0xB6,0x56,0x4E,0x71,0x6C,0x7A,0x9C,0x2B,0x00,0xE2,
     0x7C,0xAD,0x96,0x54,0x01,0x6F,0x51,0xA8,0x32,0x58,0x4D,0x5A,0x44,0x74,0xEC,0xE5,
     0xA7,0xCD,0xA1,0x93,0xD7,0x7B,0x82,0xF7,0xAE,0xAE,0x01,0x65,0x66,0x31,0x02,0xFD,
     0xAB,0xE0,0x44,0x35 };

void dump_hex(unsigned char *pBuf, int len);
void ds_demo(void);

void dump_hex(unsigned char *pBuf, int len)
{
    int i = 0;
    PRINTF(" { ");
    for (i=0; i<16; i++)
    {
        PRINTF("0x%02x,", pBuf[i]&0xff);
    }
    if (len > 16) {
        PRINTF("......");
    }
    PRINTF(" }\n\r\n\r");
}

// generate random seed
void ds_seed_init(void)
{
    srand((unsigned int)time(0));
    for(int i = 0; i < DS_SEED_LEN; i++){
        seed_src[i] = (unsigned char)rand() & 0xff;
    }
}

unsigned char *ds_rseed;
// The random callback function to be called by MASQ_DS_keypair() and MASQ_DS_sign()
int32_t ds_rand_cf(DS_RAND_HANDLE *rand_handle, int32_t rand_length,  uint8_t *rand_num)
{
    int i;

    if (rand_num == NULL) return 0;

    if (rand_handle == NULL)
    {
        // If no rand data structure used, no good way to pass infomation from ds_rand_seed_cf to ds_rand_cf
        for (i = 0; i < rand_length; i++)
        {
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
    } else {
        // The structure RAND_HANDLE is used to hold the random seed
        if (rand_handle->idx > rand_handle->DS_SEED_LENgth) {
            rand_handle->idx = 0;
        }
        for (i = 0; i < rand_length; i++)
        {
            srand(rand_handle->seed[(rand_handle->idx+i) % rand_handle->DS_SEED_LENgth]);
            rand_num[i] = (uint8_t) rand() & 0xff;
        }
        rand_handle->idx += rand_length;
    }
//    PRINTF_buf_hex("random", rand_num, rand_length);
    return 1;
}

// random seed callback function to be called by MASQ_DS_seed()
int32_t ds_rand_seed_cf(DS_RAND_HANDLE *rand_handle, uint8_t *seed, int32_t DS_SEED_LENgth)
{
    int seedlen;

    if (seed == NULL) return 0;

    if(rand_handle == NULL){
        ds_rseed = seed;
    } else {
        if (DS_SEED_LENgth > DS_SEED_LEN) {
            seedlen = DS_SEED_LEN;
        } else {
            seedlen = DS_SEED_LENgth;
        }
        memcpy(rand_handle->seed, seed, (size_t)seedlen);
        rand_handle->DS_SEED_LENgth = seedlen;
        rand_handle->idx = 0;
    }

    return 1;
}

void ds_demo(void)
{
    MASQ_API ret;
    MASQ_DS_handle *ds_handle;
    int32_t len_pk, len_sk, len_sign;
    unsigned char *pk=NULL, *sk=NULL, *sign_msg=NULL;

    DS_RAND_HANDLE ds_rand_handle;

    // Initialize DS data structure
    // We are going to use NIST level 5, you can also change it to Level 3 or Level 1 with lower security
    // The random callback function ds_rand_cf() will be called later by MASQ_DS_keypair and MASQ_DS_sign
    // The random seed callback function ds_rand_seed_cf() will be called later by MASQ_ds_seed
    // The DS_RAND_HANDLE and ds_rand_handle will be used to pass information from ds_rand_seed_cf to ds_rand_cf
    // If ds_rand_handle is NULL, there is no good way to pass information
    // Using ds_rand_handle may give user a chance to generate random number with more possibility
    ds_handle = MASQ_DS_init(LEVEL5, (MASQ_rand_callback_t)ds_rand_cf, (MASQ_rand_seed_callback_t)ds_rand_seed_cf, (struct MASQ_RAND_HANDLE_ *)&ds_rand_handle);
    if (ds_handle == NULL) {
        goto err;
    }

    // generate random seed
    ds_seed_init();

    // Call ds_rand_seed_cf() with random seed passed in
    ret = MASQ_ds_seed(ds_handle, seed_src, DS_SEED_LEN);
    if(ret != MASQ_SUCCESS) {
        goto err;
    }

    // get the length of the public key, private key and signature
    len_pk = MASQ_DS_public_key_length(ds_handle);
    len_sk = MASQ_DS_private_key_length(ds_handle);
    len_sign = MASQ_DS_signature_length(ds_handle);

    PRINTF("pk_len: %d, sk_len:%d, sign_len:%d \n\r", len_pk, len_sk, len_sign );
    pk = malloc((size_t)len_pk);
    sk = malloc((size_t)len_sk);
    sign_msg = malloc((size_t)len_sign*4);

    // generate your key pair and release your public key to public
    ret = MASQ_DS_keypair(ds_handle, pk, sk);
    if(ret != MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Keypairs not generated\n\r");
        goto err;
    }
    else{
        PRINTF("MASQ DS Unit Test: Keypairs Generated\n\r");
    }
    PRINTF("\n\rPublic key (length %d): ", len_pk);
    dump_hex(pk, len_pk);
    PRINTF("Secrete key (length %d): ", len_sk);
    dump_hex(sk, len_sk);

    PRINTF("\n\rLength of message: %d ", MSG_LEN);
    // Sign the message with your private key and get the signature in the sign_msg
    ret = MASQ_DS_sign(ds_handle, sk, msg, MSG_LEN, sign_msg, &len_sign);
    if(ret != MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Signature not generated\n\r");
        goto err;
    }
    else{
        PRINTF("MASQ DS Unit Test: Signature Generated\n\r");
    }

    PRINTF("Message (MSG_LEN %d): ", MSG_LEN);
    dump_hex(msg, MSG_LEN);
    PRINTF("Signature (len_sign %d): ", len_sign);
    dump_hex(sign_msg, len_sign);

    // People can use your public key to verify the signature to make sure it's signed by you
    ret = MASQ_DS_verify(ds_handle, pk, msg, MSG_LEN, sign_msg, len_sign);
    if(ret == MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Passed Signature Verification\n\r\n\r");
        goto err;
    }
    else {
        PRINTF("MASQ DS Unit Test: Signature not Verified %d\n\r\n\r", ret);
    }

err:
    MASQ_DS_free(ds_handle);
    if (pk != NULL) free(pk);
    if (sk != NULL) free(sk);
    if (sign_msg != NULL) free(sign_msg);
}
