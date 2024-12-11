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
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "masq_ds.h"
#ifdef USE_DILITHIUM_RANDOM
#include "dilithium_randombytes.h"
#endif

/* 
 * This demo displays one example of the Digital Signature API, specifically level 5,
 * to sign a message between two parties(Alice & Bob) 
 *
 * In this case, Alice wants to send a secret message to Bob. 
 * So, Alice begins by generating a public-private keypair.
 * Alice uses its own private key and generate the signature. 
 * Alice sends Bob, the public key and the signature. 
 * Bob then uses Alice's public key to verify the message and signature. 
 * Thus, both Alice and Bob arrive at the same message.
 */


/* Customize the random callback function below according to your specific needs. 
 * Below is template only.
 * rand_handle is NULL for this callback functions. You can add your own structure handle to pass.
 */
int32_t rand_cf(MASQ_RAND_handle rand_handle, int32_t rand_length,  uint8_t *rand_num){
    
    if (rand_num == NULL) return 0;
    
    for (int i = 0; i < rand_length; i++)
    {
        rand_num[i] = rand() & 0xff;
    }
    return 1;
}

/* Customize the seed callback function below according to your specific needs. 
 * Below is template only.
 * rand_handle is NULL for this callback function. You can add your own structure handle to pass.
 */
int32_t rand_seed_cf(MASQ_RAND_handle rand_handle, uint8_t *seed, int32_t seed_length)
{
    if (seed == NULL) return 0;

    unsigned int sd = 0;
    for (int i=0; i < seed_length; i++)
    {
        sd ^= seed[i];
        srand(sd);
    }
    return 1;
}

/* This function generate random input message string of size to sign */
static unsigned char *rand_string(unsigned char *str, size_t size)
{
    const unsigned char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK0123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

/* Initialize/set seed for any random number generator used by Alice and Bob
* We recommend to call QiSpace API (i.e. QE) to get seed 
* if no good quantum safe level entropy source */

int32_t gen_rand_seed_bob(uint8_t *seed, int32_t seed_length)
{
    /* here we just use std lib rand to set seed for demo purpose */
    srand(time(0));
    for(int j = 0; j < seed_length; j++){
            seed[j] = rand() & 0xff;
    }
    return 1;
}

int32_t gen_rand_seed_alice(uint8_t *seed, int32_t seed_length)
{
    /* here we just use std lib rand to set seed for demo purpose */
    srand(time(0));
    for(int j = 0; j < seed_length; j++){
            seed[j] = rand() & 0xff;
    }
    return 1;
}


void sendPubKey(){
    printf("\n*Alice sends Bob her public key and signature..*\n");
}

int main(int argc, const char * argv[]) {

    MASQ_DS_handle *ds_handle_alice, *ds_handle_bob;
    int32_t len_pk_alice, len_sk_alice, len_signature;
    unsigned char *pk_alice, *sk_alice, *signature;
    unsigned char alice_seed[1024], bob_seed[1024];
    int seed_len = 32;
    int len_msg = 1024;
    unsigned char *msg = malloc(len_msg + 1);
    if(msg)
    {
        /* Generating random message */
        rand_string(msg, len_msg);
    }


    /* Init: Initialize MASQ DS handles for Alice and Bob */ 
    printf("\nInitialize MASQ DS handle for Alice and Bob...\n");

#ifndef USE_PQRND
#ifdef USE_DILITHIUM_RANDOM
    /* Uses Dilithium Default Random Generator */
    ds_handle_bob = MASQ_DS_init(DS_LEVEL5, dilithium_rand_cf, dilithium_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    ds_handle_bob = MASQ_DS_init(DS_LEVEL5, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen) */
    ds_handle_bob = MASQ_DS_qeep_init(DS_LEVEL5);
#endif 
    
    if(ds_handle_bob == NULL) {
        printf("\nMASQ DS Handle init failed for Bob.\n");    
        return -1;    
    }

#ifndef USE_PQRND
#ifdef USE_DILITHIUM_RANDOM
    /* Uses Dilithium Default Random Generator */
    ds_handle_alice = MASQ_DS_init(DS_LEVEL5, dilithium_rand_cf, dilithium_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    ds_handle_alice = MASQ_DS_init(DS_LEVEL5, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen) */
    ds_handle_alice = MASQ_DS_qeep_init(DS_LEVEL5);
#endif
    
    if(ds_handle_alice == NULL) {
        printf("\nMASQ DS Handle init failed for Alice.\n");    
        return -1;
    }
    printf("\nSuccess!\n");


    /** Set the Seed for random number generator for Bob*/
    gen_rand_seed_bob(bob_seed, seed_len);
    MASQ_DS_seed(ds_handle_bob, bob_seed, seed_len);
    /** Set the Seed for random number generator for Alice*/
    gen_rand_seed_alice(alice_seed, seed_len);
    MASQ_DS_seed(ds_handle_alice, alice_seed, seed_len);

    /* Create Alice's pk_alice & sk_alice keypair */
    printf("\nCreate Alice's keypair...\n");
    len_pk_alice = MASQ_DS_public_key_length(ds_handle_alice);
    len_sk_alice = MASQ_DS_private_key_length(ds_handle_alice);
    pk_alice = malloc(len_pk_alice);
    sk_alice = malloc(len_sk_alice);

    /* Setup Alice's signature */
    len_signature = MASQ_DS_signature_length(ds_handle_alice);

    signature = malloc(len_signature);
   
    /* Generate key pair*/
    MASQ_DS_keypair(ds_handle_alice, pk_alice, sk_alice);  
    printf("\nSuccess!\n");


    /* Alice: Uses own private key to sign the message */
    printf("\nAlice uses own private key to sign the message...\n");
    MASQ_DS_sign(ds_handle_alice, sk_alice, msg, len_msg, signature, &len_signature);
    printf("\nSuccess!\n");

    /** Send Alice's public key and signature to Bob **/
    sendPubKey();

    /* Bob: Uses Alice's public key and verifies message and signature */
    printf("\nBob uses Alice's public key to verify the message and signature...\n");
    if(MASQ_DS_verify(ds_handle_bob, pk_alice, msg, len_msg, signature, len_signature) == 0)
    {
        printf("\nSuccess: Signature Verification Passed.\n");
    }
    else {
        printf("\nFailure: Signature Verification Failed.\n");
    }
    printf("\nThe secret message Bob received from Alice: \n\n[%s]\n", msg);
 
    /** Free MASQ DS Handle */
#ifndef USE_PQRND
    MASQ_DS_free(ds_handle_alice);
    MASQ_DS_free(ds_handle_bob);
#else
    MASQ_DS_qeep_free(ds_handle_alice);
    MASQ_DS_qeep_free(ds_handle_bob);
#endif
    free(pk_alice);
    free(sk_alice);
    free(signature);
    free(msg);

    printf("\n--------------------------------------------------------\n\n");
    
    return 0;
}