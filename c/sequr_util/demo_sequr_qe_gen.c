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
#include "sequr_util.h"
#include "help_util.h"


#define MAX_KEY_BUF_SIZE  2048
#define IV_SIZE           16

void print_usage() {
    printf( "Usage: ./demo_sequr_qe_gen [-h] [--qispace_meta QISPACE_META] [--length len] \n");
    printf( "\nOptions:\n");
    printf( "  -h, --help        Show this help message and exit\n");
    printf( "  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n");
    printf( "  --length          Length of required QE in byte\n");
}

int main(int argc, char *argv[]) {

    char *qispace_meta_path = NULL;
    char *qispace_meta = NULL;
    uint8_t *QE = NULL;
    int ret, qe_len = 10;

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
        } else if (strcmp(argv[i], "--length") == 0 && i + 1 < argc) {
            qe_len = atoi(argv[++i]);
        } else {
            printf( "Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    if (qispace_meta_path == NULL) {
        printf( "Error: qispace_meta_path is not provided.\n");
        print_usage();
        return 1;
    }

    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta == NULL) { 
        printf( "Error: reading qispace_meta file.\n");
        return 1; 
    }

    // init sequr_util
    sequr_handle *handle = sequr_util_init(qispace_meta);
    if(!handle){
        printf( "Error: failed to Initialize SEQUR Util.\n");
        return 1;
    }

    QE = malloc(qe_len);
    if (QE == NULL) {
        printf("Error: memory allocation for QE failed.\n");
        sequr_free(handle);
        return 1;
    }

    // query qe 
    ret = sequr_util_get_qe(handle, QE, qe_len);
    if(ret != qe_len) {
        printf( "Error: failed to query qe.\n");
        sequr_free(handle);
        return 1;
    } else {
        printf( "------------------------\n");
        printf( "QE query successful.\n");
        printf( "QE: ");
        for (int i = 0; i < qe_len; i++) {
            printf("%02x", QE[i]);
        }
    }
    printf( "\n------------------------\n");
    
    free(qispace_meta);
    free(QE);
    // free sequr handle
    sequr_free(handle);

    return 0;
}
