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
#include <iomanip>
#include "sequr_util.h"
#include "help_util.h"

using namespace std;

#define MAX_KEY_BUF_SIZE  2048

void print_usage() {
    std::cout << "Usage: ./demo_sequr_key_query [-h] [--qispace_meta QISPACE_META] [--key_id KEY_ID] [--key_type KEY_TYPE]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help        Show this help message and exit\n";
    std::cout << "  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n";
    std::cout << "  --key_id          Key ID to query\n";
    std::cout << "  --key_type        0: AES key, 1: QEEP Key,2: QEEP PASS, default: AES Key\n";
}

int main(int argc, char *argv[]) {
    const char *qispace_meta_path = nullptr;
    std::string qispace_meta;
    std::string key_id;
    int key_type = 0;
    uint8_t key[MAX_KEY_BUF_SIZE];

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
        } else if (strcmp(argv[i], "--key_id") == 0 && i + 1 < argc) {
            key_id = argv[++i];
        } else if (strcmp(argv[i], "--key_type") == 0 && i + 1 < argc) {
            key_type = atoi(argv[++i]);
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            print_usage();
            return 1;
        }
    }

    if (qispace_meta_path == nullptr) {
        std::cerr << "Error: qispace_meta_path is not provided.\n";
        print_usage();
        return 1;
    }
    if (key_id.empty()) {
        std::cerr << "Error: key_id is missing.\n";
        print_usage();
        return 1;
    }

    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta.empty()) { 
        std::cerr << "Error: reading qispace_meta file failed.\n";
        return 1; }

    // init sequr_util
    SequrHandle *handle = sequr_util_init(qispace_meta);
    if(!handle){
        std::cerr << "Error: failed to initialize SEQUR Util.\n";
        return 1;
    }

    // query key by key_id
    int key_size = sequr_util_query_key(handle, key_id, key, key_type);
    if(key_size <= 0) {
        std::cerr << "Error: failed to query key.\n";
        delete handle;
        return 1;
    }
    std::cout << "------------------------\n";
    std::cout << "Key query successful.\n";
    std::cout << "Key ID: " << key_id.c_str() << "\n";
    std::cout << "Key: ";
    if (key_type == (int)QEEP_PASS) {
        std::cout <<"QISPACE:QK:";
    }
    std::cout << setfill('0');
    for (int i = 0; i < key_size; i++) {
        cout << std::hex <<setw(2) << (int)key[i];
    }
 
    std::cout << "\n------------------------\n";

    // free sequr handle
    sequr_free(handle);

    return 0;
}

