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

#include "masq_ds.h"
#include "masq_ds_error.h"

// Redefine this as print_to_console() when usb_pcdc works
#define PRINTF(...)

unsigned char seed_src[32] =
    {0xC8,0x0E,0x6B,0xE7,0xBA,0xD6,0xA9,0xE0,0xF4,0x84,0x49,0x03,0x63,0xA1,0xB1,0xA6,
     0x21,0xAA,0xFE,0x87,0x3E,0x9F,0xE6,0x15,0xA3,0x95,0x9E,0x95,0xBB,0x13,0xB6,0xF5};

#define MSG_LEN 1024
#define SEED_LEN 32

typedef struct _randds_handle {
    int seed_length;
    uint8_t seed[SEED_LEN];
} RAND_HANDLE;

uint8_t msg[MSG_LEN] =
    {0xF1,0x25,0xEE,0x6A,0xB9,0x21,0x7F,0x07,0x91,0x52,0x65,0x7B,0x11,0xB3,0x58,0x21,0xA2,0xAA,0xAF,0xCF,0x22,0xCE,0x60,0x8A,0x6A,0xE2,0xA0,0x66,0xC6,0x22,0xAA,0xCA,0x76,0xDA,0x87,0xF2,0xF2,0x21,0x16,0xB3,0x5A,0xA2,0x28,0x21,0xE7,0x6F,0xB7,0xD1,0xA0,0xA2,0x0C,0x0F,0x6F,0x42,0xB6,0x56,0x4E,0x71,0x6C,0x7A,0x9C,0x2B,0x00,0xE2,0x7C,0xAD,0x96,0x54,0x01,0x6F,0x51,0xA8,0x32,0x58,0x4D,0x5A,0x44,0x74,0xEC,0xE5,0xA7,0xCD,0xA1,0x93,0xD7,0x7B,0x82,0xF7,0xAE,0xAE,0x01,0x65,0x66,0x31,0x02,0xFD,0xAB,0xE0,0x44,0x35,0xCE,0x4C,0x3E,0xC3,0x7C,0xE7,0xB9,0x75,0x08,0xBD,0x1D,0xED,0xED,0x93,0xE9,0xAB,0x4C,0xAB,0x0E,0x5A,0x43,0x04,0x1A,0x38,0x6C,0xE3,0x3B,0x63,0x87,0x79,0x34,0x51,0x18,0xC6,0xEA,0x63,0x95,0x88,0x5A,0xAE,0xB8,0x86,0xC1,0x6F,0x90,0xCB,0xD4,0x6F,0x62,0x35,0x15,0xA4,0x87,0x82,0x16,0x38,0x8F,0x17,0x1A,0xA7,0x56,0xAE,0xE7,0x57,0x96,0xD6,0xCC,0x90,0x5F,0x20,0x3B,0x66,0x4D,0xD0,0x3F,0xE3,0xC0,0x0D,0x54,0x36,0x62,0x42,0x19,0xFF,0xB2,0x75,0x9D,0x47,0xA2,0xBA,0x3D,0x7B,0xBB,0x47,0x0F,0xF1,0xF4,0x35,0xA9,0x8A,0xB7,0x89,0xC6,0x9D,0x0F,0x0D,0x29,0x22,0x29,0x45,0x7A,0x7D,0xEB,0xBE,0x72,0xDC,0x90,0x27,0x92,0x5E,0x1A,0x62,0x42,0xA1,0xB1,0xE0,0x5A,0x23,0xD7,0x93,0x64,0x56,0xF9,0x4F,0x85,0xD2,0xC0,0x9B,0x29,0xD8,0x19,0x08,0x1E,0x09,0x71,0x38,0x9D,0xB6,0x4B,0x9B,0x39,0x69,0xA0,0x5C,0x8A,0x3C,0xAF,0x8B,0xB9,0x28,0x1A,0x90,0x14,0xD5,0x2F,0x73,0xB6,0x57,0x42,0x03,0xE2,0x6B,0x8C,0x99,0x64,0x7F,0x11,0x2C,0x29,0x17,0x79,0x5A,0xF7,0xEC,0xB8,0xB9,0xCE,0xF2,0xC8,0x38,0xCF,0x10,0xDD,0x26,0xD2,0xE7,0xD7,0xAE,0x88,0x12,0x2A,0x3F,0xF0,0x0A,0xD9,0xFA,0x75,0xA4,0xE7,0x73,0x70,0x1A,0xF3,0x96,0x49,0x96,0xAD,0x5C,0xEF,0xFE,0xDC,0xDF,0x47,0x67,0x15,0x6E,0xA6,0xD5,0x79,0xFB,0xA4,0x71,0x7A,0x00,0x40,0xC2,0xF4,0xEF,0x2C,0x00,0xBE,0xE9,0x35,0xBA,0xEB,0x48,0x40,0x61,0x63,0x68,0x48,0xF5,0xDA,0x9E,0xFA,0x74,0xED,0x58,0x86,0x27,0x0C,0x22,0xBE,0x89,0x71,0x17,0x58,0xCE,0xAD,0xAE,0x2E,0x72,0x9F,0x8E,0xC0,0xBE,0x49,0x01,0x6D,0xEB,0xCA,0xD2,0xCA,0x4D,0xEF,0x16,0x9C,0xBF,0xF9,0x75,0x25,0x6A,0x38,0x7E,0x0C,0x28,0x32,0x57,0x53,0xB0,0x20,0x4F,0xC5,0x16,0xB5,0x15,0x99,0x11,0x91,0xF5,0x18,0x31,0x89,0xED,0x55,0x76,0xEF,0x1E,0xCA,0x2D,0x28,0xB1,0x4D,0x9D,0x6D,0x78,0x32,0x5B,0x15,0x80,0xEB,0x7C,0x3C,0xF7,0x06,0x04,0xFB,0x78,0xB3,0xCE,0x09,0x30,0xEF,0xEE,0xE7,0x1D,0xA5,0x84,0x6C,0xB8,0x60,0xCB,0xFD,0x85,0xFE,0x4D,0xDE,0x26,0x86,0xEC,0x1D,0xE5,0x09,0xED,0x6D,0x73,0x0F,0x80,0x7A,0xCC,0x6F,0x52,0x49,0xAC,0x0D,0xF5,0xC6,0x1E,0xA0,0x4A,0x61,0xC7,0x3C,0x41,0x33,0xC0,0x2E,0xEF,0x08,0x0C,0x3C,0x73,0x21,0x43,0xA7,0xE4,0x86,0xE9,0x44,0x50,0x90,0xCB,0xCF,0xE7,0x59,0x37,0x5A,0x93,0x5A,0xFE,0xA5,0x7D,0xE0,0x8B,0x70,0x8C,0xB6,0x4B,0xD4,0xFA,0xFD,0xBF,0x51,0x30,0xC7,0x6F,0x12,0xC1,0xC8,0xFA,0xA6,0x7F,0x2B,0xD6,0x7B,0xF7,0x79,0x1B,0xD2,0xDC,0x8A,0xC8,0x9D,0x88,0x7A,0x15,0xD8,0x84,0xFA,0xFE,0x3E,0x24,0x9A,0x70,0x7F,0xAD,0x3B,0xD2,0x96,0x99,0x8E,0x35,0x69,0xC4,0x86,0xA6,0x59,0xEA,0xC6,0x5F,0xA2,0x5E,0xE7,0x4E,0xEB,0x79,0x68,0x55,0xA5,0x3B,0x8A,0xD4,0x22,0x13,0xC8,0xE3,0xEB,0x1A,0x8C,0x9E,0xD7,0x72,0xDB,0xF6,0x13,0x80,0x7E,0xA2,0x00,0x8A,0xEA,0x10,0x79,0x21,0x0D,0x5F,0xD1,0x99,0xB5,0xC7,0x8A,0xB1,0xFA,0xAA,0x82,0xD0,0xCB,0x19,0xAF,0x2F,0x01,0x4F,0xF8,0x84,0x06,0xB4,0xFF,0xAE,0x76,0x89,0xD1,0x3A,0x6C,0xC9,0xDC,0x64,0x01,0x9A,0x46,0xA5,0xCD,0xDB,0xFF,0xCE,0xE4,0x5E,0xCF,0x39,0x64,0x0A,0xD8,0xB3,0x22,0x60,0x7F,0x3F,0x08,0x8C,0xC8,0x7B,0xD6,0x30,0xC0,0x64,0x98,0xC6,0x6A,0x03,0xB4,0x3D,0x5C,0x21,0xB8,0xB7,0x5D,0xE0,0x4B,0x62,0xCC,0x6F,0x86,0x33,0x8E,0xE5,0x95,0xC3,0x71,0xA5,0x8A,0x59,0x2F,0xB6,0x9F,0xA1,0xDD,0x95,0x7E,0xB8,0xED,0xDC,0x06,0xE5,0x6F,0xFC,0x5B,0xBD,0xCB,0xCD,0xF0,0x52,0x2D,0x27,0xD9,0x0E,0x1A,0x9B,0x67,0x21,0xCF,0x86,0xBD,0x11,0x4D,0x65,0x4E,0x84,0x67,0x18,0x25,0x4A,0xD8,0x9F,0x84,0x74,0x72,0x00,0x5F,0x8D,0x73,0x7D,0x90,0x07,0x4F,0xF0,0x5C,0xF6,0xE5,0x0D,0xC7,0xAB,0x6F,0x00,0x8E,0x5B,0xEB,0xBF,0xB0,0x0F,0x24,0xFB,0xC0,0xCF,0x84,0x8B,0xDA,0xF9,0xF3,0x4C,0xC0,0x61,0x8A,0x68,0x93,0xAB,0x2E,0x47,0xC0,0xAB,0xA9,0xCA,0x97,0x3C,0x3D,0xCA,0xE8,0x72,0xD3,0xEB,0x26,0xFB,0xDF,0x12,0xD2,0x09,0x35,0x90,0x57,0x63,0x94,0xE1,0x92,0x48,0x20,0xCB,0x6B,0x72,0x38,0x2B,0x9D,0x2D,0x95,0x39,0xD8,0x7C,0xCD,0x5E,0x4E,0x09,0xD0,0xF7,0x78,0x40,0xFE,0x05,0x90,0x18,0x3F,0xE4,0xDB,0x5F,0x10,0xA6,0x0A,0xCE,0x97,0x5F,0x2B,0x86,0x40,0x50,0x44,0x88,0x57,0xF0,0x8D,0x51,0x49,0x9E,0x3D,0x03,0x65,0x5F,0xC5,0x1B,0x5B,0x04,0x89,0xCA,0xCA,0xAD,0x6E,0xE4,0xC5,0xEB,0x4E,0x6D,0x22,0xE3,0x0C,0x65,0x79,0x34,0x4D,0x0D,0xDA,0x26,0x81,0xAB,0x74,0x1B,0xD4,0x6C,0x50,0x0D,0xCC,0x1E,0xA7,0x3B,0xDA,0x99,0x03,0x4B,0xE9,0xE3,0x64,0x58,0x45,0x87,0x84,0x8E,0x48,0x6E,0x16,0xB3,0xAE,0x40,0xF8,0x28,0x59,0x89,0x59,0x43,0x29,0x41,0xA4,0x6C,0x28,0xFD,0xB5,0x75,0x16,0xC2,0x94,0xFF,0x4A,0x12,0x4B,0xF7,0xB2,0x8E,0x9C,0x23,0x5C,0x82,0x13,0xBE,0xE8,0xF3,0x58,0x58,0x5A,0xAF,0xFE,0x6F,0xEE,0x44,0x31,0x17,0xB0,0x5F,0x81,0x88,0xDA,0x4E,0x3F,0x1F,0x56,0xB9,0x0E,0x82,0x36,0x7A,0x0C,0xE7,0x9B,0xF7,0x9E,0x69,0x0F,0xD2,0xF1,0xF6,0x75,0x46,0x58,0x3A,0xB5,0x82,0x99,0xA5,0x55,0x45,0x7E,0x2D,0x9F,0x57,0xB9,0x6B,0xA5,0x3D,0x83,0x40,0xB7,0x31,0x8B,0xCF,0xBC,0x4C,0x90,0xEA,0xA9,0xBF,0xFB,0x93,0x05,0xC5,0xD5,0xAE,0xF6,0x5E,0x2C,0x75,0x29,0xBC,0xBB,0x33,0xED,0x8A,0xE6,0xC6,0xE1,0xD4,0x67,0xBB};

void dump_hex(unsigned char *pBuf, int len);
int ds_demo(void);

void dump_hex(unsigned char *pBuf, int len)
{
    int i = 0;
    PRINTF(" { ");
    for (i=0; i<len; i++)
    {
        PRINTF("0x%02x,", pBuf[i]&0xff);
    }
    PRINTF(" }\n\r\n\r");
}

int32_t randds_cf(RAND_HANDLE *randds_handle, int32_t randds_length,  uint8_t *randds_num)
{
    int i;

    if (randds_num == NULL) return 0;

    if (randds_handle == NULL)
    {
        for (i = 0; i < randds_length; i++)
        {
            randds_num[i] = (uint8_t) rand() & 0xff;
        }
    } else {
        for (i = 0; i < randds_length; i++)
        {
            //srand(randds_handle->seed[i%randds_handle->seed_length]);
            randds_num[i] = (uint8_t) rand() & 0xff;
        }
    }
    return 1;
}

int32_t randds_seed_cf(RAND_HANDLE *randds_handle, uint8_t *seed, int32_t seed_length)
{
    if (seed == NULL) return 0;

    if(randds_handle == NULL){
       seed = seed_src;
    } else {
        memcpy(randds_handle->seed, seed_src, seed_length);
        randds_handle->seed_length = seed_length;
    }

    return 1;
}

int ds_demo(void)
{
    MASQ_API ret0 = 1, ret1 = 1, ret2 = 1, ret3 = 1;
    MASQ_DS_handle *ds_handle;
    int32_t len_pk, len_sk, len_sign;
    unsigned char *pk=NULL, *sk=NULL, *sign_msg=NULL;

    RAND_HANDLE randds_handle = {0, {0}};

    ds_handle = MASQ_DS_init(LEVEL5, randds_cf, randds_seed_cf, (struct MASQ_RAND_HANDLE_ *)&randds_handle);
    if (ds_handle == NULL) {
        goto bail;
    }

    ret0 = MASQ_ds_seed(ds_handle, seed_src, SEED_LEN);
    if(ret0 != MASQ_SUCCESS) {
        goto bail;
    }

    len_pk = MASQ_DS_public_key_length(ds_handle);
    len_sk = MASQ_DS_private_key_length(ds_handle);
    len_sign = MASQ_DS_signature_length(ds_handle);

    PRINTF("pk_len: %d, sk_len:%d, sign_len:%d \n\r", len_pk, len_sk, len_sign );
    pk = malloc((size_t)len_pk);
    sk = malloc((size_t)len_sk);
    sign_msg = malloc((size_t)len_sign*4);

    ret1 = MASQ_DS_keypair(ds_handle, pk, sk);
    if(ret1 != MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Keypairs not generated\n\r");
        goto bail;
    }
    else{
        PRINTF("MASQ DS Unit Test: Keypairs Generated\n\r");
    }
    PRINTF("\n\rPublic key (length %d): ", len_pk);
    dump_hex(pk, len_pk);
    PRINTF("Secrete key (length %d): ", len_sk);
    dump_hex(sk, len_sk);

    PRINTF("\n\rLength of message: %d ", MSG_LEN);
    ret2 = MASQ_DS_sign(ds_handle, sk, msg, MSG_LEN, sign_msg, &len_sign);
    PRINTF("sign time: %d - %d = %d(ms) \r\n", tickend, tickstart, tickend-tickstart);
    if(ret2 != MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Signature not generated\n\r");
        goto bail;
    }
    else{
        PRINTF("MASQ DS Unit Test: Signature Generated\n\r");
    }

    PRINTF("Message (MSG_LEN %d): ", MSG_LEN);
    dump_hex(msg, MSG_LEN);
    PRINTF("Signature (len_sign %d): ", len_sign);
    dump_hex(sign_msg, len_sign);

    ret3 = MASQ_DS_verify(ds_handle, pk, msg, MSG_LEN, sign_msg, len_sign);
    if(ret3 == MASQ_SUCCESS) {
        PRINTF("MASQ DS Unit Test: Passed Signature Verification\n\r\n\r");
        goto bail;
    }
    else {
        PRINTF("MASQ DS Unit Test: Signature not Verified %d\n\r\n\r", ret);
    }

bail:
    MASQ_DS_free(ds_handle);
    if (pk != NULL) free(pk);
    if (sk != NULL) free(sk);
    if (sign_msg != NULL) free(sign_msg);

    if (ret0==MASQ_SUCCESS && ret1==MASQ_SUCCESS && ret2==MASQ_SUCCESS && ret3==MASQ_SUCCESS) {
        return 0;
    } else {
        return 1;
    }
}
