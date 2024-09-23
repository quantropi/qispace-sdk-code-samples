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

void print_usage() {
    printf("Usage: ./demo_sequr_key_gen [-h] [--qispace_meta QISPACE_META] [--key_size_bits KEY_SIZ_BITS]\n");
    printf("\nOptions:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n");
    printf("  --key_size_bits   Key size to generate (in bits)\n");
}

char *read_file_to_string(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to store the file contents + null terminator
    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL) {
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t read_size = fread(file_content, 1, file_size, file);
    if (read_size != file_size) {
        free(file_content);
        fclose(file);
        return NULL;
    }

    file_content[file_size] = '\0';  // Null-terminate the string

    fclose(file);
    return file_content;
}

int main(int argc, char *argv[]) {
    char *qispace_meta_path = NULL;
    char *qispace_meta = NULL;
    int keysize = 256;

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--qispace_meta") == 0 && i + 1 < argc) {
            qispace_meta_path = argv[++i];
        } else if (strcmp(argv[i], "--key_size_bits") == 0 && i + 1 < argc) {
            keysize = atoi(argv[++i])/8;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!qispace_meta_path || keysize <= 0) {
        printf("Error: Missing required arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta == NULL) { return 1; }

    // init sequr_util
    sequr_handle *handle = sequr_util_init(qispace_meta);
    if(handle == NULL){
        printf("Error: failed to initialize sequr util\n");
        return 1;
    }

    // generate key 
    char key_id[256];
    uint8_t key[keysize];
    int key_size = sequr_util_key_gen(handle, keysize, key_id, key);
    if(key_size <= 0) {
         printf("Error: failed to generate key\n");
        return 1;
    }
    printf("------------------------\n");
    printf("Key generation successful\n");
    printf("Key ID: %s\n", key_id);
    printf("Key: ");
    for (int i = 0; i < 5; i++) {
        printf("%02x ", key[i]);
    }
    printf("... ");
    for (int i = key_size - 5; i < key_size; i++) {
        printf("%02x ", key[i]);
    }
    printf("\n------------------------\n");

    // free sequr handle
    sequr_free(handle);

    return 0;
}

