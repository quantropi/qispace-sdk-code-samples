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
    std::cout << "Usage: ./demo_sequr_qe_gen [-h] [--qispace_meta QISPACE_META] [--length len] \n";
    std::cout << "\nOptions:\n";
    std::cout << "  -h, --help        Show this help message and exit\n";
    std::cout << "  --qispace_meta    Path to qispace meta .json file, provided by Quantropi Inc.\n";
    std::cout << "  --length          Length of required QE in byte\n";
}

int main(int argc, char *argv[]) {

    const char *qispace_meta_path = nullptr;
    std::string qispace_meta;
    uint8_t *QE = NULL;
    int ret, len = 10;

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
            len = atoi(argv[++i]);
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

    // read file 
    qispace_meta = read_file_to_string(qispace_meta_path);
    if(qispace_meta.empty()) { 
        std::cerr << "Error: reading qispace_meta file.\n";
        return 1; 
    }

    // init sequr_util
    SequrHandle *handle = sequr_util_init(qispace_meta);
    if(!handle){
        std::cerr << "Error: failed to Initialize SEQUR Util.\n";
        return 1;
    }

    QE = new uint8_t[len];

    // query qe 
    ret = sequr_util_get_qe(handle, QE, len);
    if(ret != len) {
        std::cerr << "Error: failed to query qe.\n";
        sequr_free(handle);
        return 1;
    } else {
        std::cout << "------------------------\n";
        std::cout << "QE query successful.\n";
        std::cout << "QE: ";
        std::cout << setfill('0');
        for (int i = 0; i < len; i++) {
            cout << std::hex <<setw(2) << (int)QE[i];
        }
    }
    std::cout << "\n------------------------\n";

    // free sequr handle
    sequr_free(handle);
    delete[] QE;

    return 0;
}
