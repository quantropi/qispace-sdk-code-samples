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
#include "help_util.h"
#include "sequr_util.h"

void print_usage(void) {
    printf("Usage: ./demo_sequr_key_gen [-h] [--qispace_meta QISPACE_META] [--key_size_bits KEY_SIZ_BITS] [--key_type KEY_TYPE]\n");
    printf("\nOptions:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n");
    printf("  --key_size_bits   Key size to generate (in bits)\n");
    printf("  --key_type        0: AES key, 1: QEEP Key, 2: QEEP Pass, default: AES Key\n");
}


int main(int argc, char *argv[]) {
    char *qispace_meta_path = NULL;
    char *qispace_meta = NULL;
    int   keysize_bits = 256;
    int   key_type = 0;
    char  key_id[256];
    uint8_t *key;

    if (argc < 2) {
        print_usage();
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--qispace_meta") == 0 && i + 1 < argc) {
            qispace_meta_path = argv[++i];
        } else if (strcmp(argv[i], "--key_size_bits") == 0 && i + 1 < argc) {
            keysize_bits = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--key_type") == 0 && i + 1 < argc) {
            key_type = atoi(argv[++i]);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    if (!qispace_meta_path) {
        printf("Error: qispace_meta_path is not provided.\n");
        print_usage();
        return 1;
    }
    if (keysize_bits <=0) {
        printf("Error: key_size_bits is invalid.\n");
        print_usage();
        return 1;
    }
    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta == NULL) { 
        printf("Error: reading qispace meta file failed.\n");
        return 1; }

    // init sequr_util
    sequr_handle *handle = sequr_util_init(qispace_meta);
    if(handle == NULL){
        printf("Error: failed to initialize SEQUR Util.\n");
        return 1;
    }

    // generate key 
    key  = (uint8_t *) malloc(sequr_util_key_buff_size(keysize_bits/8, key_type));
    if (key == NULL) {
        printf("Error: memory allocation for key failed.\n");
        sequr_free(handle);
        return 1;
    }

    int key_size = sequr_util_key_gen(handle, keysize_bits/8, key_id, key, key_type);
    if(key_size <= 0) {
        printf("Error: failed to generate key.\n");
        sequr_free(handle);
        free(key);
        return 1;
    }
    printf("------------------------\n");
    printf("Key generation successful.\n");
    printf("Key ID: %s\n", key_id);
    printf("Key: ");
    if (key_type == (int)QEEP_PASS) {
        printf("QISPACE:QK:");
    }
    for (int i = 0; i < key_size; i++) {
        printf("%02x", key[i]);
    }
    printf("\n------------------------\n");
    free(key);
    free(qispace_meta);
    // free sequr handle
    sequr_free(handle);

    return 0;
}

