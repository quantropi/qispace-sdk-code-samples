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
#include <time.h>
#include <string.h>
#include "qispace_sequr.h"

int Q_printf(const char *format, ...);
#define PRINTF Q_printf

int sequr_demo(void);

#define DEMO_QEEP_KEY_BUFFER_SIZE         1024
#define DEMO_MESSAGE_SIZE                 1024
#define DEMO_IV_SIZE                      16

static uint8_t HexChar (char c)
{
    if ('0' <= c && c <= '9') return (uint8_t)(c - '0');
    if ('A' <= c && c <= 'F') return (uint8_t)(c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (uint8_t)(c - 'a' + 10);
    return 0xFF;
}

static int HexToBin (const char* s, uint8_t *buff, int length)
{
    int result;
    if (!s || !buff || length <= 0) return -1;

    for (result = 0; *s; ++result)
    {
        uint8_t msn = HexChar(*s++);
        if (msn == 0xFF) return -1;
        uint8_t lsn = HexChar(*s++);
        if (lsn == 0xFF) return -1;
        uint8_t bin = (uint8_t)(msn << 4) + lsn;

        if (length-- <= 0) return -1;
        *buff++ = bin;
    }
    return result;
}

static char qeep_key_str[]="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";
static uint8_t cipher[]="SQUR demo cipher, should be generated by QiSpace Server";


int sequr_demo(void)
{
    QEEP_RET ret = QEEP_OK;
    QSC_Handle qsc_handle_dec;

    uint8_t plaintext[DEMO_MESSAGE_SIZE];
    uint8_t qeep_key[DEMO_QEEP_KEY_BUFFER_SIZE];

    int32_t qeep_key_len = (int32_t)strlen((char*)qeep_key_str)/2;
    int32_t cipher_length = (int32_t)strlen((char*)cipher)/2;
    uint8_t iv[DEMO_IV_SIZE]="1234567890123456";

    qeep_key_len = (int32_t)strlen(qeep_key_str)/2;

    /* convert qeep key from hex string to binary array */
    if (HexToBin(qeep_key_str,qeep_key, qeep_key_len ) != qeep_key_len) {
        PRINTF("Convert QK hex string to binary array fail \n");
        return (1);
    }

/** 
 * ##################################################################
*   Use QEEP key to decode cipher  
*  ##################################################################
*/

    PRINTF("Demo SQUR decode operation ....\r\n");

/**
*    Step 1, Create QSC class handle
*/
    ret = QSC_init(&qsc_handle_dec);
    if (ret != QEEP_OK) { PRINTF("QSC_init fail\r\n"); return (-1);}

/**
 *    Step 2, Load QEEP decode key
 *    The QSC_qeep_key_load() will load both encrypt key and decrypt key
 *    The QSC_qeep_key_load_de() load only decrypt key, it will save about 32KB memory usage
 */
    ret = QSC_qeep_key_load_de(qsc_handle_dec, qeep_key, qeep_key_len);
    if (ret != QEEP_OK) { PRINTF("QSC_qeep_key_load fail ret = %d\r\n", ret); return (-1);}

/**
 *    Step 3, Load IV 
 */
    ret = QSC_qeep_iv_set(qsc_handle_dec, iv, DEMO_IV_SIZE);
    if (ret != QEEP_OK) { PRINTF("QSC_qeep_iv_set fail\r\n"); return (-1);}

/**
 *    Step 4, Decrypt operation 
 */
    ret = QSC_qeep_decode(qsc_handle_dec, cipher,  cipher_length, plaintext);
    if (ret != QEEP_OK) { PRINTF("QSC_qeep_decrypt fail\r\n"); return (-1);}

/**
*    Step 5, Free QSC class handle
*/
    ret = QSC_close(qsc_handle_dec);
    PRINTF("Done \r\n");

    return 0;
}
