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

/*
 * This demo demonstrates the usage of PQRND API to generate some
 * pseudo quantum random data after being seeded with a pseudorandom number and displays it
 * 
 * Enabling QISPACE_API_ON causes a retrieval of Quantum Entropy (QE) from QiSpace 
 * Enterprise's REST API to use as seed instead with the unpacking process also
 * illustrated as well. The function responsible for the HTTP GET request is implemented in RestApiUtil.
 * 
 * The unpacking process of the QE utilizes the SEQUR API as it is a payload from QiSpace Enterprise,
 * so viewing the SEQUR demo prior is recommended.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "../lib/qispace_pqrnd.h"

#include "base64.h"
#ifdef QISPACE_API_ON
#include "RestApiUtil.h"
#include "../lib/qispace_sequr.h"
#endif

#define RANDOM_SEED_LEN                     64
#define RANDOM_OUT_LEN                      32
#define DEMO_SUB_KEY_BUFFER_SIZE            3000
#define DEMO_PAYLOAD_BUFFER_SIZE            3000
#define IV_BUFFER_SIZE                      IV_SIZE*2
#define IV_SIZE                             16
#define QE_BUFFER_SIZE                      QE_SIZE*2
#define QE_SIZE                             32

static unsigned char rnd_seed[RANDOM_SEED_LEN], rnd_out[RANDOM_OUT_LEN];

int main(int argc, char **argv) 
{
    QEEP_RET ret = QEEP_OK;
    int i;
    
    #ifdef QISPACE_API_ON
    char    iv_base64[IV_BUFFER_SIZE];                      
    uint8_t iv[IV_SIZE];                                    
    int32_t iv_len;                                         

    char    sub_key_hex[DEMO_SUB_KEY_BUFFER_SIZE];          
    uint8_t sub_key[DEMO_SUB_KEY_BUFFER_SIZE];              
    int32_t sub_key_len = 0;                                

    char    QE_base64[QE_BUFFER_SIZE];                      
    int32_t QE_len;                                         
    uint8_t encoded_QE[QE_SIZE];                            
    #endif

    printf("Demo SEQUR PQRND operations...\n");

/**
*    Step 1: Create SEQUR PQRND handle
*/
    printf("initializing pQrnd handle...\n");
    pQrnd_Handle qrnd_handle;
    ret = pQrndInit(&qrnd_handle); 
    if (ret != QEEP_OK) {
        printf("pQrndInit fail,  ret = %d \n", ret);
        return (-1);
    }
    printf("success!\n");


/**
*  Step 2: Generate PQRND seed
*/
    /*
        If QISPACE_API_ON defined, call QiSpace API to get QE as the seed. 
        Otherwise, generate random seed with built-in C library rand 
    */
    #ifdef QISPACE_API_ON
        /* retrieve subkey from QiSpace Enterprise */
        printf("retrieving subkey...\n");
        ret = QiSpaceAPI_subKey( sub_key_hex );
        
        /* On success, convert subkey format from hex string to binary array */
        if (ret == 1) {
            printf("success!\n");
            sub_key_len = strlen(sub_key_hex)/2;
            for(i = 0; i< sub_key_len; i++)
                sscanf((char*)&sub_key_hex[2*i], "%02x", (unsigned int *)&sub_key[i]);
        }else {
            printf("QiSpaceAPI_subKey fail: %d \n", ret);
            return(-1);
        }

        /* retrieve QE from QiSpace Enterprise */
        printf("retrieving qe...\n");
        ret = QiSpaceAPI_QE( QE_SIZE, QE_base64, iv_base64 );

        /* On success, decode QE and IV from base64 string to byte array */
        if (ret == 1) {
            printf("success!\n");
            iv_len = base64_decode(iv_base64, strlen(iv_base64), iv);
            QE_len = base64_decode(QE_base64, strlen(QE_base64), encoded_QE);
        }
        else{
            printf("QiSpaceAPI_QE fail: %d \n", ret);
            return(-1);
        }

        /* setup SEQUR API to decode payload (i.e. QE) with subkey and seed PQRND with it */
        printf("decoding QE...\n");
        QSC_Handle qsc_handle_dec;
        ret = QSC_init(&qsc_handle_dec);
        if (ret != QEEP_OK) { printf("QSC_init fail ret = %d\n", ret); return (-1);}

        ret = QSC_qeep_key_load(qsc_handle_dec, sub_key, sub_key_len);
        if (ret != QEEP_OK) { printf("QSC_qeep_key_load fail ret = %d\n", ret); return (-1);}

        ret = QSC_qeep_iv_set(qsc_handle_dec, iv, iv_len);
        if (ret != QEEP_OK) { printf("QSC_qeep_iv_set fail ret = %d\n", ret); return (-1);}

        ret = QSC_qeep_decode(qsc_handle_dec, encoded_QE,  QE_len, rnd_seed);    
        if (ret != QEEP_OK) { printf("QSC_qeep_decrypt fail ret = %d\n", ret); return (-1);}

        QSC_close(qsc_handle_dec);
        printf("success!\n");
    #else
        /* setup a random seed */
        srand(time(0));
        for(i = 0; i< RANDOM_SEED_LEN; i++)
            rnd_seed[i] = rand()&0xff;
        
    #endif

/**
*    Step 3: Load PQRND seed (with QE if QISPACE_API_ON enabled)
*/
    printf("loading pqrnd seed...\n");
    ret = pQrndSeed(qrnd_handle, rnd_seed, RANDOM_SEED_LEN);
    if (ret != QEEP_OK) {
        printf("pQrndSeed fail,  ret = %d \n", ret);
        return (-1);
    }
    printf("success!\n");

/**
*    Step 4: Generate pseudo quantum random data and display
*/
    printf("generating pqrnd...\n");
    ret = pQrndRnd(qrnd_handle, rnd_out, RANDOM_OUT_LEN);
    if (ret != QEEP_OK) {
        printf("pQrndRnd fail,  ret = %d \n", ret);
        return (-1);
    }
    printf("success!\n");

    printf("pQrndRnd Rnd: ");
    for (i = 0; i < RANDOM_OUT_LEN ; i++ ) {
        printf(" %02X ", rnd_out[i]);
    }
    printf("\n");

/**
*    Step 5: Free PQRND handle  
*/
    ret = pQrndClose(qrnd_handle);
    printf("done!");
    printf("\n--------------------------------------------------------\n\n");
    return (0);
}

