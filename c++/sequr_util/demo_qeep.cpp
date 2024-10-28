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
#include <iomanip>
#include "qispace_qeep.h"

/*
 * This demo demonstrates an example of QEEP API's encrypt and decrypt functionality
 * applied on a message string if "-m" is provided with a pseudo-random IV.
 * The Quantum Key is loaded into QEEP API if provided via "-k".
 * The default pre-loaded in hex string is used if "-k" not provided.
 * 
*/

using namespace std;

#define DEMO_IV_SIZE                      16
#define DEMO_MESSAGE_SIZE                 32

void print_usage(char *name) {
    std::cout << "Usage: " << name <<" [-h] [-k qeep_key] [-m message]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help        Show this help message and exit\n";
    std::cout << "  -k                qeep key hex string\n";
    std::cout << "  -m                message string to encrypt\n";
}

int main(int argc, char **argv)
{
        QEEP_RET ret = QEEP_OK;
        int i;

        QP_Handle qp_handle_enc;
        QP_Handle qp_handle_dec;
  
        uint8_t iv[DEMO_IV_SIZE]; 

        uint8_t *message;
        int32_t message_len = DEMO_MESSAGE_SIZE;
        uint8_t *encrypted_message, *decrypted_message;
        int32_t encrypted_message_len = 0;

        std::string QK_str_hex="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";

        uint8_t *QK;
        int32_t QK_len = 0;
        std::string M_str="";

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        print_usage(argv[0]);
                        return 0;
                } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
                        QK_str_hex = argv[++i];
                } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
                        M_str = argv[++i];
                } else {
                        std::cerr << "Unknown option: " << argv[i] << "\n";
                        print_usage(argv[0]);
                        return 1;
                }
        }

        if (QK_str_hex.length() == 0 || QK_str_hex.length()/2 * 2 != QK_str_hex.length()) {
                std::cerr << "Error: Qeep Key Hex String is invalid.\n";
                print_usage(argv[0]);
                return 1;
        }

        srand(static_cast<unsigned int>(time(0)));

        /* convert qeep key from hex string to byte array */
        QK_len = QK_str_hex.length()/2;
        QK = new uint8_t[QK_len];
        for(i = 0; i < QK_len; i++){ 
                sscanf(QK_str_hex.c_str() + 2*i, "%02hhx", &QK[i]);
        }

        /* generate random test IV per message - must be 16 Bytes */
        for(i = 0; i < DEMO_IV_SIZE; i++){
                iv[i] = rand()&0xff;
        }

        /* create random message for test purpose */
        if (M_str.length() > 0) {
                message_len = M_str.length();
                message = new uint8_t[message_len];
                M_str.copy((char*)message,message_len, 0 );
        } else {
                //random generate default message
                message = new uint8_t[message_len];
                for(i = 0; i< message_len; i++){
                        message[i] = rand()&0xff;
                }
        }

        encrypted_message = new uint8_t[message_len];
        decrypted_message = new uint8_t[message_len];

/** 
 * ##################################################################
*   Use QEEP key to encode message 
*  ##################################################################
*/

        std::cout << "Demo QiSpace QEEP Encode operation...\n" ;
/**
*    Step 1: Create QEEP handle
*/
        std::cout << "Creating QEEP handle...\n" ;
        ret = QP_init(&qp_handle_enc);
        if (ret != QEEP_OK) { std::cout << "QP_init fail: " << ret ; goto DEMO_END;}
        std::cout << " Success!\n" ;

/**
 *    Step 2: Load QEEP key. 
 */

        /* load the qeep key for eventual QEEP encryption/decryption */
        std::cout << "Loading QEEP key...\n" ;
        ret = QP_qeep_key_load(qp_handle_enc, QK, QK_len);
        if (ret != QEEP_OK) { std::cout << "QP_qeep_key_load fail: " << ret << "\n"; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
 *    Step 3: Load message IV 
 */
        std::cout << "Loading message IV...\n" ;
        ret = QP_iv_set(qp_handle_enc, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { std::cout << "QP_iv_set fail:  " << ret ; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
 *    Step 4: Encrypt message 
 */
        std::cout << "Encrypting the message...\n" ;
        ret = QP_encrypt(qp_handle_enc, message,  message_len, encrypted_message);
        if (ret != QEEP_OK) { std::cout << "QP_encode fail: " << ret ; goto DEMO_END;}
        encrypted_message_len = message_len;
        std::cout << "Success!\n" ;

/**
*    Step 5: Free encoding QEEP handle
*/
        ret = QP_close(qp_handle_enc);
        std::cout << "Demo QiSpace QEEP Encode operation done.\n\n" ;


/** 
 * ##################################################################
*   Use QEEP key to decode cipher 
*  ##################################################################
*/

        std::cout << "Demo QiSpace QEEP decode operation...\n" ;
/**
*    Step 1: Create QEEP handle
*/
        std::cout << "Creating QEEP handle...\n" ;
        ret = QP_init(&qp_handle_dec);
        if (ret != QEEP_OK) { std::cout << "QP_init fail: " << ret << "\n"; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
 *    Step 2: Load QEEP key
 */
        std::cout << "Loading QEEP key...\n" ;
        ret = QP_qeep_key_load(qp_handle_dec, QK, QK_len);
        if (ret != QEEP_OK) { std::cout << "QP_qeep_key_load fail: " << ret << "\n"; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
 *    Step 3: Load IV 
 */
        std::cout << "Loading message IV...\n" ;
        ret = QP_iv_set(qp_handle_dec, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { std::cout << "QP_iv_set fail: " << ret << "\n"; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
 *    Step 4: Decrypt operation 
 */
        std::cout << "Decrypting the encrypted message...\n" ;
        ret = QP_decrypt(qp_handle_dec, encrypted_message,  encrypted_message_len, decrypted_message);
        if (ret != QEEP_OK) { std::cout << "QP_decrypt fail: " << ret << "\n"; goto DEMO_END;}
        std::cout << "Success!\n" ;

/**
*    Step 5: Free QEEP handle
*/
        ret = QP_close(qp_handle_dec);
        std::cout << "Demo QiSpace QEEP Decode operation done.\n\n" ;


/**
 *   CHECK: Comparing initial message and decrypted plaintext for testing purposes
 */
        std::cout << "Compare message and decrypted message...\n" ;
        if(!memcmp(message, decrypted_message, message_len)) {
                std::cout << "Match: decrypted_message = message.\n" ;
        } else {
                std::cout << "No match: decrypted_message != message. ret: " << ret << "\n";
        }
        std::cout << "Input message: \n  ";
        std::cout << setfill('0');
        for (int i = 0; i < message_len; i++) {
                cout << std::hex <<setw(2) << (int)message[i];
        }
        std::cout << "\nEncrypted message: \n  ";
        std::cout << setfill('0');
        for (int i = 0; i < message_len; i++) {
                cout << std::hex <<setw(2) << (int)encrypted_message[i];
        }
        std::cout << "\nDecrypted message: \n  ";
        std::cout << setfill('0');
        for (int i = 0; i < message_len; i++) {
                cout << std::hex <<setw(2) << (int)decrypted_message[i];
        }
        std::cout << "\n--------------------------------------------------------\n\n" ;
DEMO_END:
        delete[] message;
        delete[] encrypted_message;
        delete[] decrypted_message;
        return 0;
}