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
 * This demo demonstrates an example of QEEP to encrypt and decrypt a file
 * 
*/

#define IV_SIZE                      16
#define DEFAULT_BLOCK_SIZE           16

static char QK_str_hex_default[]="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";
static char IV_hex_default[]="59e86830efc1c6559d315fd635bd7e59";

void print_usage(char *name) {
    printf("Usage: %s [-h] <-in input_filename> <-out output_filename> [-k qeep_key] [-iv initial_vector] [-bs block_size]  [-enc 1|0 ] \n", name);
    printf("\nOptions:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  -k                qeep key hex string\n");
    printf("  -iv               initial vector hex string\n");
    printf("  -bs               block size in bytes, default: %d \n", DEFAULT_BLOCK_SIZE);
    printf("  -in               input full path file name\n");
    printf("  -out              output full path file name\n");
    printf("  -enc [1|0]        1: encode or 0:decode, default is 1 \n");
}

int main(int argc, char **argv)
{
        QEEP_RET ret = QEEP_OK;
        int i;

        QP_Handle qp_handle;
  
        uint8_t iv[IV_SIZE]; 

        uint8_t *message;
        int32_t block_size = DEFAULT_BLOCK_SIZE;
        uint8_t *encode_message;
        int32_t read_message_len = 0;
        int32_t file_size =0;
        FILE *fileIn=NULL;
        FILE *fileOut=NULL;
        int enc = 1;

        char *pQKstr = QK_str_hex_default;
        char *pIVstr = IV_hex_default;
        uint8_t *QK;
        int32_t QK_len = 0;


        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
                pQKstr = argv[++i];
            } else if (strcmp(argv[i], "-iv") == 0 && i + 1 < argc) {
                pIVstr = argv[++i];
            } else if (strcmp(argv[i], "-bs") == 0 && i + 1 < argc) {
                block_size =  strtol(argv[++i], NULL, 10);
            } else if (strcmp(argv[i], "-enc") == 0 && i + 1 < argc) {
                enc =  strtol(argv[++i], NULL, 10);
                if (enc != 0 && enc != 1) {
                    printf("-enc input [%d] is invalid, must be 1 or 0.\n", enc);
                    print_usage(argv[0]);
                    return 1;
                }
            } else if (strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
                fileIn = fopen(argv[++i], "rb");
                if (fileIn == NULL) {
                    printf(" open input filename failed\n");
                    print_usage(argv[0]);
                    return 1;
                }
            } else if (strcmp(argv[i], "-out") == 0 && i + 1 < argc) {
                fileOut = fopen(argv[++i], "wb");
                if (fileOut == NULL) {
                    printf(" open output filename failed\n");
                    print_usage(argv[0]);
                    return 1;
                }
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
        if (strlen(pIVstr) == 0 || strlen(pIVstr)/2 * 2 != strlen(pIVstr)) {  
                printf("Error: IV Hex String is invalid.\n");
                print_usage(argv[0]);
                return 1;
        }

        if (fileIn == NULL || fileOut == NULL) {
                printf("Error: filename for -in and -out must be provided! \n\n");
                print_usage(argv[0]);
                return 1;
        }
        /* convert qeep key from hex string to byte array */
        QK_len = strlen(pQKstr)/2;
        QK = malloc(QK_len);
        for(i = 0; i < QK_len; i++) 
            sscanf((char*)&pQKstr[2*i], "%02x", (unsigned int *)&QK[i]);
        

        /* convert  IV per message - must be 16 Bytes */
        for(i = 0; i < IV_SIZE; i++)
            sscanf((char*)&pIVstr[2*i], "%02x", (unsigned int *)&iv[i]);

        message = malloc(block_size);
        encode_message = malloc(block_size);



/** 
 * ##################################################################
*   Use QEEP to Encode or Decode file 
*  ##################################################################
*/


/*
*    Step 1: Create QEEP handle
*/

        ret = QP_init(&qp_handle);
        if (ret != QEEP_OK) { printf("QP_init fail ret =%d\n", ret); goto DEMO_END;}


/**
 *    Step 2: Load QEEP key. 
 */

        /* load the qeep key for eventual QEEP encryption/decryption */

        ret = QP_qeep_key_load(qp_handle, QK, QK_len);
        if (ret != QEEP_OK) { printf("QP_qeep_key_load fail ret = %d\n", ret);  goto DEMO_END;}


/**
 *    Step 3: Load message IV 
 */

        ret = QP_iv_set(qp_handle, iv, IV_SIZE);
        if (ret != QEEP_OK) { printf("QP_iv_set fail ret =%d\n", ret);  goto DEMO_END;}


/**
 *    Step 4: Encode/Decode file per block size
 */
        if (fileIn != NULL && fileOut != NULL) {
            while((read_message_len = fread(message, 1, block_size, fileIn)) > 0 ) {
                if (enc) {
                    ret = QP_encrypt(qp_handle, message,  read_message_len, encode_message);
                } else {
                    ret = QP_decrypt(qp_handle, message,  read_message_len, encode_message);
                }
                if (ret != QEEP_OK) { printf("QP_encode/QP_decrypt fail ret =%d\n", ret);  goto DEMO_END;}
                if (fwrite(encode_message, 1, read_message_len, fileOut) != read_message_len ) {
                    printf("fwtite fileOut fail \n");  goto DEMO_END;
                }
                file_size += read_message_len;
            }


        }

/**
*    Step 5: Free  QEEP handle
*/
        ret = QP_close(qp_handle);
        printf("file[%d bytes] Qeep encoded successsfully \n",file_size );


DEMO_END:
        free(message);
        free(encode_message);
        free(QK);
        if (fileIn != NULL ) fclose(fileIn);
        if (fileOut != NULL) fclose(fileOut);
        return 0;
}