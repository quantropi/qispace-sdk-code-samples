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
#include "qispace_qeep.h"



/*
 * This demo demonstrates an example of QEEP API's encrypt and decrypt functionality
 * applied on a message string if "-m" is provided with a pseudo-random IV.
 * The Quantum Key is loaded into QEEP API if provided via "-k".
 * The default pre-loaded in hex string is used if "-k" not provided.
 * 
*/

#define DEMO_IV_SIZE                      16
#define DEMO_MESSAGE_SIZE                 32

void print_usage(char *name) {
    printf("Usage: %s [-h] [-k qeep_key] [-m message]\n", name);
    printf("\nOptions:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  -k                qeep key hex string\n");
    printf("  -m                message string to encode\n");
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

        char QK_str_hex[]="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";
        char *pQKstr = QK_str_hex;
        uint8_t *QK;
        int32_t QK_len = 0;
        char *M_str = NULL;

        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
                pQKstr = (argv[++i]);
            } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
                M_str = argv[++i];
            } else {
                printf("Unknown option: %s\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
        }

        if (strlen(pQKstr) == 0 || strlen(pQKstr)/2 * 2 != strlen(pQKstr)) {  
                printf("Error: Qeep Key Hex String is invalid.\n");
                print_usage(argv[0]);
                return 1;
        }

        srand(time(0));

        /* convert qeep key from hex string to byte array */
        QK_len = strlen(QK_str_hex)/2;
        QK = malloc(QK_len);
        for(i = 0; i < QK_len; i++) 
            sscanf((char*)&QK_str_hex[2*i], "%02x", (unsigned int *)&QK[i]);
        

        /* generate random test IV per message - must be 16 Bytes */
        for(i = 0; i < DEMO_IV_SIZE; i++)
            iv[i] = rand()&0xff;
        
        if (M_str == NULL) {
            //random generate
            message_len = DEMO_MESSAGE_SIZE;
            message = malloc(message_len);
            for(i = 0; i< message_len; i++)
                message[i] = rand()&0xff;
        } else {
            message_len = strlen(M_str);
            message = malloc(message_len);
            memcpy(message, M_str, message_len);
        }

        encrypted_message = malloc(message_len);
        decrypted_message = malloc(message_len);


/** 
 * ##################################################################
*   Use QEEP key to Encode message 
*  ##################################################################
*/

        printf("Demo QiSpace QEEP Encode operation ...\n");

/*
*    Step 1: Create QEEP handle
*/
        printf("Creating QEEP handle...\n");
        ret = QP_init(&qp_handle_enc);
        if (ret != QEEP_OK) { printf("QP_init fail ret =%d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 2: Load QEEP key. 
 */

        /* load the qeep key for eventual QEEP encryption/decryption */
        printf("Loading QEEP key...\n");
        ret = QP_qeep_key_load(qp_handle_enc, QK, QK_len);
        if (ret != QEEP_OK) { printf("QP_qeep_key_load fail ret = %d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 3: Load message IV 
 */
        printf("Loading message IV...\n");
        ret = QP_iv_set(qp_handle_enc, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { printf("QP_iv_set fail ret =%d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 4: Encode message 
 */
        printf("Encoding message...\n");
        ret = QP_encrypt(qp_handle_enc, message,  message_len, encrypted_message);
        if (ret != QEEP_OK) { printf("QP_encode fail ret =%d\n", ret);  goto DEMO_END;}
        encrypted_message_len = message_len;
        printf("Success!\n");

/**
*    Step 5: Free Encrypting QEEP handle
*/
        ret = QP_close(qp_handle_enc);
        printf("Demo QiSpace QEEP Encode operation done!\n\n");


/** 
 * ##################################################################
*   Use QEEP key to Decode cipher 
*  ##################################################################
*/

        printf("Demo QiSpace QEEP Decode operation...\n");
/**
*    Step 1: Create QEEP handle
*/
        printf("Creating QEEP handle...\n");
        ret = QP_init(&qp_handle_dec);
        if (ret != QEEP_OK) { printf("QP_init fail ret =%d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 2: Load QEEP key
 */
        printf("Loading QEEP key...\n");
        ret = QP_qeep_key_load(qp_handle_dec, QK, QK_len);
        if (ret != QEEP_OK) { printf("QP_qeep_key_load fail ret = %d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 3: Load IV 
 */
        printf("Loading message IV...\n");
        ret = QP_iv_set(qp_handle_dec, iv, DEMO_IV_SIZE);
        if (ret != QEEP_OK) { printf("QP_iv_set fail ret =%d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
 *    Step 4: Decode operation 
 */
        printf("Decoding the encoded message...\n");
        ret = QP_decrypt(qp_handle_dec, encrypted_message,  encrypted_message_len, decrypted_message);
        if (ret != QEEP_OK) { printf("QP_decrypt fail ret = %d\n", ret);  goto DEMO_END;}
        printf("Success!\n");

/**
*    Step 5: Free QEEP handle
*/
        ret = QP_close(qp_handle_dec);
        printf("Demo QiSpace QEEP Decode operation done.\n\n");


/**
 *   CHECK: Comparing initial message and decrypted plaintext for testing purposes
 */
        printf("Compare message and decrypted message...\n");
        if(!memcmp(message, decrypted_message, message_len)) {
            printf("-Match: decrypted_message = message.\n");
        } else {
            printf("-No match: decrypted_message != message.\n");
        }
        printf("-Input message[%d]: \n ", message_len);
        for (int i = 0; i < message_len; i++) {
            printf("%02X", message[i]);
        }
        printf("\n-Encrypted message: \n");
        for (int i = 0; i < message_len; i++) {
            printf("%02X", encrypted_message[i]);
        }
        printf("\n-Decrypted message: \n");
        for (int i = 0; i < message_len; i++) {
            printf("%02X", decrypted_message[i]);
        }
        printf("\n--------------------------------------------------------\n\n");
DEMO_END:
        free(message);
        free(encrypted_message);
        free(decrypted_message);
        free(QK);
        return 0;
}