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
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <ctime>
#include "qispace_qeep.h"
#include "base64.h"
#ifdef QISPACE_API_ON
#include "RestApiUtil.h"
#include "qispace_sequr.h"
#endif


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

using namespace std;

#define DEMO_SUB_KEY_BUFFER_SIZE          3000
#define DEMO_QK_BUFFER_SIZE               256
#define DEMO_IV_SIZE                      16
#define DEMO_IV_BUFFER_SIZE               50
#define DEMO_MESSAGE_SIZE                 1024
#define QK_NUM_BYTES                      32

int main(int argc, char **argv)
{
        QEEP_RET ret = QEEP_OK;
        int i;

        QP_Handle qp_handle_enc;
        QP_Handle qp_handle_dec;

        #ifdef QISPACE_API_ON

        std::string iv_base64;         
        int32_t iv_len;                                 

        std::string sub_key_hex;     
        uint8_t sub_key[DEMO_SUB_KEY_BUFFER_SIZE];      
        int32_t sub_key_len = 0;                        

        std::string QK_base64;      
        uint8_t encoded_QK[DEMO_QK_BUFFER_SIZE];   

        int int_ret = 0;
        #endif

        uint8_t iv[DEMO_IV_SIZE]; 

        uint8_t message[DEMO_MESSAGE_SIZE];
        int32_t message_len = DEMO_MESSAGE_SIZE;
        uint8_t encrypted_message[DEMO_MESSAGE_SIZE], decrypted_message[DEMO_MESSAGE_SIZE];
        int32_t encrypted_message_len = 0;

        int encoded_QK_len = QK_NUM_BYTES+22;
        uint8_t QK[DEMO_QK_BUFFER_SIZE];
        int32_t QK_len = 0;
        std::string QK_str="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";

        srand(static_cast<unsigned int>(time(0)));

        /* convert qeep key from hex string to byte array */
        QK_len = QK_str.length()/2;
        for(i = 0; i < QK_len; i++) 
                sscanf(QK_str.c_str() + 2*i, "%02hhx", &QK[i]);
        

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


        std::cout << "Demo QiSpace QEEP operation...\n" ;
/**
*    Step 1: Create QEEP handle
*/
        std::cout << "Creating QEEP handle...\n" ;
        ret = QP_init(&qp_handle_enc);
        if (ret != QEEP_OK) { std::cout << "QP_init fail: %d\n" << ret ; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 2: Load QEEP key. If QISPACE_API_ON, obtain a fresh QK from QiSpace Enterprise first
 */

        #ifdef QISPACE_API_ON
        /* retrieve sub_key and QK from QiSpace Enterprise */
        std::cout << "Retrieving subkey from QiSpace Enterprise...\n" ;
        int_ret = QiSpaceAPI_subKey( sub_key_hex);
        /* If subkey successfully retrieved from QiSpace API, convert from hex string to byte array */
        if (int_ret == 1) {
                std::cout << "Success!\n" ; 
                sub_key_len = sub_key_hex.length()/2;

                /* convert retrieved subkey from hex string to byte array */
                for(i = 0; i< sub_key_len; i++) {
                        sscanf(sub_key_hex.c_str() + 2*i, "%02hhx", &sub_key[i]);
                }
        }else {
                std::cout << "QiSpaceAPI_subKey fail: %d \n" << int_ret << "\n";
                return(-1);
        }       

        std::cout << "Retrieving QK from QiSpace Enterprise...\n" ; 
        int_ret = QiSpaceAPI_QK( QK_base64, iv_base64, QK_NUM_BYTES);
        /* On success, decode QE and IV from base64 string to byte array */
        if (int_ret == 1) {
            std::cout << "Success!\n" ;

            /* decode quantum key and iv from base64 format to byte array */
            iv_len = base64_decode(iv_base64, iv_base64.length(), iv);
            encoded_QK_len = base64_decode(QK_base64, QK_base64.length(), encoded_QK);
        }
        else{
            std::cout << "QiSpaceAPI_QE fail: %d \n" << int_ret << "\n";
            return(-1);
        }


        /* SEQUR decode the retrieved QK using retrieved sub_key */
        std::cout << "Decoding QK...\n" ;
        QSC_Handle qsc_handle_dec;
        ret = QSC_init(&qsc_handle_dec);
        if (ret != QEEP_OK) { std::cout << "QSC_init fail: %d\n" << ret << "\n"; return (-1);}

        ret = QSC_qeep_key_load(qsc_handle_dec, sub_key, sub_key_len);
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_key_load fail: %d\n" << ret << "\n"; return (-1);}

        ret = QSC_qeep_iv_set(qsc_handle_dec, iv, iv_len);
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_iv_set fail: %d\n" << ret << "\n"; return (-1);}

        ret = QSC_qeep_decode(qsc_handle_dec, encoded_QK,  encoded_QK_len, QK);    
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_decrypt fail: %d\n" << ret << "\n"; return (-1);}

        QSC_close(qsc_handle_dec);
        std::cout << "Success!\n" ;        

        #endif

        /* load the qeep key for eventual QEEP encryption/decryption */
        std::cout << "Loading QEEP key...\n" ;
        ret = QP_qeep_key_load(qp_handle_enc, QK, encoded_QK_len);
        if (ret != QEEP_OK) { std::cout << "QP_qeep_key_load fail: %d\n" << ret << "\n"; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 3: Load message IV 
 */
        std::cout << "Loading message IV...\n" ;
        ret = QP_iv_set(qp_handle_enc, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { std::cout << "QP_iv_set fail: %d \n" << ret ; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 4: Encrypt message 
 */
        std::cout << "Encrypting message...\n" ;
        ret = QP_encrypt(qp_handle_enc, message,  message_len, encrypted_message);
        if (ret != QEEP_OK) { std::cout << "QP_encode fail %d \n" << ret ; return (-1);}
        encrypted_message_len = message_len;
        std::cout << "Success!\n" ;

/**
*    Step 5: Free encoding QEEP handle
*/
        ret = QP_close(qp_handle_enc);
        std::cout << "Done \n\n" ;


/** 
 * ##################################################################
*   Use QEEP key to decode cipher 
*  ##################################################################
*/

        std::cout << "Demo QEEP decode operation...\n" ;
/**
*    Step 1: Create QEEP handle
*/
        std::cout << "Creating QEEP handle...\n" ;
        ret = QP_init(&qp_handle_dec);
        if (ret != QEEP_OK) { std::cout << "QP_init fail: %d\n" << ret << "\n"; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 2: Load QEEP key
 */
        std::cout << "Loading QEEP key...\n" ;
        ret = QP_qeep_key_load(qp_handle_dec, QK, encoded_QK_len);
        if (ret != QEEP_OK) { std::cout << "QP_qeep_key_load fail: %d\n" << ret << "\n"; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 3: Load IV 
 */
        std::cout << "Loading message IV...\n" ;
        ret = QP_iv_set(qp_handle_dec, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { std::cout << "QP_iv_set fail: %d\n" << ret << "\n"; return (-1);}
        std::cout << "Success!\n" ;

/**
 *    Step 4: Decrypt operation 
 */
        std::cout << "Decrypting encrypted message...\n" ;
        ret = QP_decrypt(qp_handle_dec, encrypted_message,  encrypted_message_len, decrypted_message);
        if (ret != QEEP_OK) { std::cout << "QP_decrypt fail: %d\n" << ret << "\n"; return (-1);}
        std::cout << "Success!\n" ;

/**
*    Step 5: Free QEEP handle
*/
        ret = QP_close(qp_handle_dec);
        std::cout << "Done. \n\n" ;


/**
 *   CHECK: Comparing initial message and decrypted plaintext for testing purposes
 */
        std::cout << "Compare message and decrypted message...\n" ;
        if(!memcmp(message, decrypted_message, message_len)) {
                std::cout << "Match. Demo exit ! \n" ;
                std::cout << "\n--------------------------------------------------------\n\n" ;
                return 0;
        } else {
                std::cout << "No match (ret = %d). Demo exit! \n" << ret << "\n";
                std::cout << "\n--------------------------------------------------------\n\n" ;
                return 1;
        }

        return 0;
}