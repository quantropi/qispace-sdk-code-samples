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

/*
 * This demo demonstrates the usage of PQRND API to generate some
 * pseudo quantum random data after being seeded with a pseudorandom number and displays it
 * 
 * We can retrieve Quantum Entropy (QE) from QiSpace 
 * Enterprise's REST API to use as seed or use pre-loaded QE in the same format.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "qispace_pqrnd.h"


#define RANDOM_SEED_LEN                     64
#define RANDOM_OUT_LEN                      32
#define DEMO_SUB_KEY_BUFFER_SIZE            3000
#define DEMO_PAYLOAD_BUFFER_SIZE            3000
#define IV_BUFFER_SIZE                      IV_SIZE*2
#define IV_SIZE                             16
#define QE_BUFFER_SIZE                      QE_SIZE*2
#define QE_SIZE                             32

static unsigned char rnd_seed[RANDOM_SEED_LEN], rnd_out[RANDOM_OUT_LEN];

void print_usage(char *name) {
    printf("Usage: %s [-h] [--qe QE] [--length len]\n", name);
    printf("\nOptions:\n");
    printf("  -h, --help        Show this help message and exit\n");
    printf("  --qe              QE hex string to seed PQRND\n");
    printf("  --length          Required length of pseudo quantum random string to generate in bytes\n");
}

static uint8_t HexChar (char c)
{
    if ('0' <= c && c <= '9') return (uint8_t)(c - '0');
    if ('A' <= c && c <= 'F') return (uint8_t)(c - 'A' + 10);
    if ('a' <= c && c <= 'f') return (uint8_t)(c - 'a' + 10);
    return 0xFF;
}

static int HexToBin (const char* s, uint8_t *buff, int length)
{
    int result;
    if (!s || !buff || length <= 0) return -1;

    for (result = 0; *s; ++result)
    {
        uint8_t msn = HexChar(*s++);
        if (msn == 0xFF) return -1;
        uint8_t lsn = HexChar(*s++);
        if (lsn == 0xFF) return -1;
        uint8_t bin = (msn << 4) + lsn;

        if (length-- <= 0) return -1;
        *buff++ = bin;
    }
    return result;
}


int main(int argc, char **argv) 
{
    int i, qe_len = 0, ret = 0;
    pQrnd_Handle qrnd_handle;
        
    /* Arbitrary payload, IV and sub_key that can be retrieve from QiSpace Enterprise */
    char qe_hex[]="495A7A73793452482B77395975496338587342755A743242726F324E646E6746533332466473336674705953424C4150735572323639354D41684964426436786E327433424163554E5461733463356B3943587664674F2B574E7475525A5445566577513353536D30486470557A556B6F41796A6570586A2B734C666C6C4F612F45427363413D3D";


    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--qe") == 0 && i + 1 < argc) {
            strcpy(qe_hex, argv[++i]);
        } else if (strcmp(argv[i], "--length") == 0 && i + 1 < argc) {
            qe_len = atoi(argv[++i]);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    printf("Demo SEQUR PQRND operations...\n");

/**
*    Step 1: Create SEQUR PQRND handle
*/
    printf("Initializing PQRND handle...\n");
    ret = pQrndInit(&qrnd_handle); 
    if (ret != QEEP_OK) {
        printf("Initializing PQRND handle failed, ret = %d \n", ret);
        return 1;
    }
    printf("Success!\n");


/**
*  Step 2: Generate PQRND seed: call QiSpace API to get QE as the seed. 
        Otherwise, use random
*/
    qe_len = strlen(qe_hex)/2;
    /* convert QE from hex string to binary array */
    if (HexToBin(qe_hex, rnd_seed, qe_len ) != qe_len) {
        printf("Convert QE hex string to binary array fail \n");
        return (-1);
    }
    printf("Success!\n");

/**
*    Step 3: Load PQRND seed
*/
    printf("Loading PQRND seed...\n");
    ret = pQrndSeed(qrnd_handle, rnd_seed, qe_len);
    if (ret != QEEP_OK) {
        printf("pQrndSeed fail,  ret = %d \n", ret);
        return 1;
    }
    printf("Success!\n");

/**
*    Step 4: Generate quantum random data and display
*/
    printf("Generating PQRND...\n");
    ret = pQrndRnd(qrnd_handle, rnd_out, qe_len);
    if (ret != QEEP_OK) {
        printf("pQrndRnd fail,  ret = %d \n", ret);
        return (-1);
    }
    printf("Success!\n");

    printf("Generated Quantum random data: ");
    for (i = 0; i < qe_len ; i++ ) {
        printf(" %02X ", rnd_out[i]);
    }
    printf("\n");

/**
*    Step 5: Free PQRND handle  
*/
    ret = pQrndClose(qrnd_handle);
    printf("Done!");
    printf("\n--------------------------------------------------------\n\n");
    return (0);
}