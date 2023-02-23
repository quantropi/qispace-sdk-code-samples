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
 * This demo demonstrates an example of QEEP API's encrypt and decrypt functionality
 * applied on a pseudo-random message with a pseudo-random IV as well. The Quantum Key loaded
 * into QEEP API is simply pre-loaded in hex string format by default.
 * 
 * However with QISPACE_API_ON enabled, the QK and IV are retrieved from QiSpace Enterprise 
 * REST API, then unpacked and loaded. The QK also requires a subkey from QiSpace Enterprise
 * in order to be SEQUR decoded thus subkey is also retrieved.
 * 
 * Since the QK unpacking process requires SEQUR API and the subkey, it is helpful to review
 * the SEQUR API demo first as the processes are directly used in the SEQUR API demo 
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../lib/qispace_qeep.h"


#define DEMO_SUB_KEY_BUFFER_SIZE          3000
#define DEMO_QK_BUFFER_SIZE         256
#define DEMO_IV_SIZE                      16
#define DEMO_IV_BUFFER_SIZE               50
#define DEMO_MESSAGE_SIZE                 1024
#define QK_NUM_BYTES                    32

#ifdef QISPACE_API_ON
#include "RestApiUtil.h"
#include "../lib/qispace_sequr.h"
#include "base64.h"
#endif

int main(int argc, char **argv)
{
        QEEP_RET ret = QEEP_OK;
        int i;

        QP_Handle qp_handle_enc;
        QP_Handle qp_handle_dec;

        #ifdef QISPACE_API_ON

        char    iv_base64[DEMO_IV_BUFFER_SIZE];         
        int32_t iv_len;                                 

        char sub_key_hex[DEMO_SUB_KEY_BUFFER_SIZE];     
        uint8_t sub_key[DEMO_SUB_KEY_BUFFER_SIZE];      
        int32_t sub_key_len = 0;                        

        char QK_base64[DEMO_QK_BUFFER_SIZE];      
        uint8_t encoded_QK[DEMO_QK_BUFFER_SIZE];   

        #endif

        uint8_t iv[DEMO_IV_SIZE]; 

        uint8_t message[DEMO_MESSAGE_SIZE];
        int32_t message_len = DEMO_MESSAGE_SIZE;
        uint8_t encrypted_message[DEMO_MESSAGE_SIZE], decrypted_message[DEMO_MESSAGE_SIZE];
        int32_t encrypted_message_len = 0;

        int encoded_QK_len = QK_NUM_BYTES+22;
        uint8_t QK[DEMO_QK_BUFFER_SIZE];
        int32_t QK_len = 0;

        // This pre-loaded QK is in hex string format just like if it were retrieved from QiSpace REST API
        uint8_t QK_str[]="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";

        srand(time(0));

        /* convert qeep key from hex string to byte array */
        QK_len = strlen((char*)QK_str)/2;
        for(i = 0; i< QK_len; i++) 
                sscanf((char*)&QK_str[2*i], "%02x", (unsigned int *)&QK[i]);
        

        /* generate random test IV per message - must be 16 Bytes */
        for(i = 0; i < DEMO_IV_SIZE; i++)
                iv[i] = rand()&0xff;

        /* create random message for test purpose */
        for(i = 0; i< message_len; i++)
                message[i] = rand()&0xff;

/** 
 * ##################################################################
*   Use QEEP key to encode message 
*  ##################################################################
*/


        printf("Demo QiSpace QEEP operation ...\n");
/**
*    Step 1: Create QEEP handle
*/
        printf("creating QEEP handle...\n");
        ret = QP_init(&qp_handle_enc);
        if (ret != QEEP_OK) { printf("QP_init fail\n"); return (-1);}
        printf("success!\n");

/**
 *    Step 2: Load QEEP key. If QISPACE_API_ON, obtain a fresh QK from QiSpace Enterprise first
 */

        #ifdef QISPACE_API_ON
                /* retrieve sub_key and QK from QiSpace Enterprise */
                printf("retrieving subkey from QiSpace Enterprise...\n");
                ret = QiSpaceAPI_subKey( sub_key_hex);
                /* If subkey successfully retrieved from QiSpace API, convert from hex string to byte array */
                if (ret == 1) {
                        printf("success!\n"); 
                        sub_key_len = strlen(sub_key_hex)/2;

                        /* convert retrieved subkey from hex string to byte array */
                        for(i = 0; i< sub_key_len; i++) {
                                sscanf((char*)&sub_key_hex[2*i], "%02x", (unsigned int *)&sub_key[i]);
                        }
                }else {
                        printf("QiSpaceAPI_subKey fail: %d \n", ret);
                        return(-1);
                }       

                printf("retrieving QK from QiSpace Enterprise...\n"); 
                ret = QiSpaceAPI_QK( QK_base64, iv_base64, QK_NUM_BYTES);
                /* On success, decode QE and IV from base64 string to byte array */
                if (ret == 1) {
                        printf("success!\n");

                        /* decode quantum key and iv from base64 format to byte array */
                        iv_len = base64_decode(iv_base64, strlen(iv_base64), iv);
                        encoded_QK_len = base64_decode(QK_base64, strlen(QK_base64), encoded_QK);
                }
                else{
                        printf("QiSpaceAPI_QE fail: %d \n", ret);
                        return(-1);
                }


                /* SEQUR decode the retrieved QK using retrieved sub_key as a key */
                printf("decoding QK...\n");
                QSC_Handle qsc_handle_dec;
                ret = QSC_init(&qsc_handle_dec);
                if (ret != QEEP_OK) { printf("QSC_init fail ret = %d\n", ret); return (-1);}

                ret = QSC_qeep_key_load(qsc_handle_dec, sub_key, sub_key_len);
                if (ret != QEEP_OK) { printf("QSC_qeep_key_load fail ret = %d\n", ret); return (-1);}

                ret = QSC_qeep_iv_set(qsc_handle_dec, iv, iv_len);
                if (ret != QEEP_OK) { printf("QSC_qeep_iv_set fail ret = %d\n", ret); return (-1);}

                ret = QSC_qeep_decode(qsc_handle_dec, encoded_QK,  encoded_QK_len, QK);    
                if (ret != QEEP_OK) { printf("QSC_qeep_decrypt fail ret = %d\n", ret); return (-1);}

                QSC_close(qsc_handle_dec);
                printf("success!\n");        

        #endif

        /* load the qeep key for eventual QEEP encryption/decryption */
        printf("loading QEEP key...\n");
        ret = QP_qeep_key_load(qp_handle_enc, QK, encoded_QK_len);
        if (ret != QEEP_OK) { printf("QP_qeep_key_load fail ret = %d\n", ret); return (-1);}
        printf("success!\n");

/**
 *    Step 3: Load message IV 
 */
        printf("loading message IV...\n");
        ret = QP_iv_set(qp_handle_enc, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { printf("fail at QP_iv_set \n"); return (-1);}
        printf("success!\n");

/**
 *    Step 4: Encrypt message 
 */
        printf("encrypting message...\n");
        ret = QP_encrypt(qp_handle_enc, message,  message_len, encrypted_message);
        if (ret != QEEP_OK) { printf("fail at QP_encode \n"); return (-1);}
        encrypted_message_len = message_len;
        printf("success!\n");

/**
*    Step 5: Free encoding QEEP handle
*/
        ret = QP_close(qp_handle_enc);
        printf("Done \n\n");


/** 
 * ##################################################################
*   Use QEEP key to decode cipher 
*  ##################################################################
*/

        printf("Demo QEEP decode operation ...\n");
/**
*    Step 1: Create QEEP handle
*/
        printf("creating QEEP handle...\n");
        ret = QP_init(&qp_handle_dec);
        if (ret != QEEP_OK) { printf("QP_init fail ret = %d\n", ret); return (-1);}
        printf("success!\n");

/**
 *    Step 2: Load QEEP key
 */
        printf("loading QEEP key...\n");
        ret = QP_qeep_key_load(qp_handle_dec, QK, encoded_QK_len);
        if (ret != QEEP_OK) { printf("QP_qeep_key_load fail ret = %d\n", ret); return (-1);}
        printf("success!\n");

/**
 *    Step 3: Load IV 
 */
        printf("loading message IV...\n");
        ret = QP_iv_set(qp_handle_dec, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { printf("QP_iv_set fail ret = %d\n", ret); return (-1);}
        printf("success!\n");

/**
 *    Step 4: Decrypt operation 
 */
        printf("decrypting encrypted message...\n");
        ret = QP_decrypt(qp_handle_dec, encrypted_message,  encrypted_message_len, decrypted_message);
        if (ret != QEEP_OK) { printf("QP_decrypt fail ret = %d\n", ret); return (-1);}
        printf("success!\n");

/**
*    Step 5: Free QEEP handle
*/
        ret = QP_close(qp_handle_dec);
        printf("Done \n\n");


/**
 *   CHECK: Comparing initial message and decrypted plaintext for testing purposes
 */
        ret = memcmp(message, decrypted_message, message_len);
        printf("Compare message and decrypted message ...\n");
        if ( ret == 0 ) {
                printf("Match. Demo exit ! \n");
                printf("\n--------------------------------------------------------\n\n");
                return 0;
        } else {
                printf("No match (ret = %d). Demo exit! \n", ret);
                printf("\n--------------------------------------------------------\n\n");
                return 1;
        }
}