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

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <curl/curl.h>
#include "sequr_util.h"
#include "help_util.h"
#include "json_util.h"


using namespace std;

void print_usage() {
    std::cout << "Usage: ./demo_sequr_key_gen [-h] [--qispace_meta QISPACE_META] [--key_size_bits KEY_SIZE_BITS]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help        Show this help message and exit\n";
    std::cout << "  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n";
    std::cout << "  --key_size_bits   Key size to generate (in bits)\n";
}


int main(int argc, char *argv[]) {
    const char *qispace_meta_path = nullptr;
    std::string qispace_meta;
    int keysize_bits = 256;
    std::string key_id;
    uint8_t *key = nullptr;

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
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            print_usage();
            return 1;
        }
    }

    if (!qispace_meta_path ) {
        std::cerr << "Error: qispace_meta_path is not provided.\n";
        print_usage();
        return 1;
    }
    if (keysize_bits <= 0 ) {
        std::cerr << "Error: key_size_bits is invalid.\n";
        print_usage();
        return 1;
    }
    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta.empty()) { 
        std::cerr << "Error: reading qispace meta file failed.\n";
        return 1; 
    }

    // init sequr_util
    SequrHandle *handle = sequr_util_init(qispace_meta);
    if(!handle){
        std::cerr << "Error: failed to initialize SEQUR Util.\n";
        return 1;
    }

    // generate key 
    key  = (uint8_t *) malloc(keysize_bits/8);
    if (!key) {
        std::cerr << "Error: memory allocation for key failed.\n";
        sequr_free(handle);
        return 1;
    }

    int key_size = sequr_util_key_gen(handle, keysize_bits/8, key_id, key);
    if(key_size <= 0) {
        std::cerr << "Error: failed to generate key.\n";
        free(key);
        sequr_free(handle);
        return 1;
    }
    std::cout << "------------------------\n";
    std::cout << "Key generation successful.\n";
    std::cout << "Key ID: " << key_id.c_str() << "\n";
    std::cout << "Key: ";
    for (int i = 0; i < key_size; i++) {
        cout << std::hex << (int)key[i] << " ";
    }
    
    std::cout << "\n------------------------\n";
    
    free(key);
    // free sequr handle
    sequr_free(handle);
    curl_global_cleanup();

    return 0;
}

