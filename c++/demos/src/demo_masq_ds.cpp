#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <ctime>
#include "masq_ds.h"
#ifdef USE_DILITHIUM_RANDOM
#include "dilithium_randombytes.h"
#endif

using namespace std;

/* 
 * This demo displays one example of the Digital Signature API, 
 * specifically level 5,  to sign a message between two parties(Alice & Bob) 
 *
 * In this case, Alice wants to send a secret message to Bob. 
 * So, Alice begins by generating a public-private keypair.
 * Alice uses its own private key and generate the signature. 
 * Alice sends Bob, the public key and the signature. 
 * Bob then uses Alice's public key to verify the message and signature. 
 * Thus, both Alice and Bob arrive at the same message.
 */

unsigned char seed_orig[1024];
int32_t seed_len = 32;  


/* 
* Initialize/set seed for any random number generator used
* We recommend to call QiSpace API (i.e. QE) to get seed 
* if no good quantum safe level entropy source
*/
int32_t seed_init()
{
    /* here we just use std lib rand to set seed for demo purpose */
    srand(static_cast<unsigned int>(time(0)));
    for(int j = 0; j < seed_len; j++){
            seed_orig[j] = rand() & 0xff;
    }
    return 1;
}

/** 
 * ##################################################################
*   Only used for Demo
*  ##################################################################
*/ 
int32_t rand_cf(MASQ_RAND_handle rand_handle, int32_t rand_length,  uint8_t *rand_num){
    
    if(rand_num == nullptr) return 0;
    
    if(rand_handle == nullptr) 
    {
        for (int i = 0; i < rand_length; i++)
        {
            rand_num[i] = rand() & 0xff;
        }
    }
    return 1;
}

int32_t rand_seed_cf(MASQ_RAND_handle rand_handle, uint8_t *seed, int32_t seed_length)
{
    if(seed == nullptr) return 0;

    if(rand_handle == nullptr){
        seed = seed_orig;
    }
    return 1;
}

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


void sendPubKey(){
    std::cout << "*Alice sends Bob her public key and signature...*\n";
}

int main(int argc, const char * argv[]) {

    MASQ_DS_handle *ds_handle_alice, *ds_handle_bob;
    int32_t len_pk_alice, len_sk_alice, len_signature;
    unsigned char *pk_alice, *sk_alice, *signature;
    int len_msg = 1024;
    unsigned char *msg = new uint8_t[len_msg + 1];
    if(msg)
    {
        rand_string(msg, len_msg);
    }

/** 
*   Initialize random seed
*/ 
    seed_init();

/** 
*  Init: Initialize MASQ DS handles for Alice and Bob
*/ 

    std::cout << "Initialize MASQ DS handle for Alice and Bob...\n";

#ifndef USE_PQRND
#ifdef USE_DILITHIUM_RANDOM
    /* Uses Dilithium Default Random Generator */
    ds_handle_bob = MASQ_DS_init(LEVEL5, dilithium_rand_cf, dilithium_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    ds_handle_bob = MASQ_DS_init(LEVEL5, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen) */
    ds_handle_bob = MASQ_DS_qeep_init(LEVEL5);
#endif    
    if(ds_handle_bob == NULL) {
        std::cout << "MASQ DS Handle init failed.\n";    
        return -1;    
    }

#ifndef USE_PQRND
#ifdef USE_DILITHIUM_RANDOM
    /* Uses Dilithium Default Random Generator */
    ds_handle_alice = MASQ_DS_init(LEVEL5, dilithium_rand_cf, dilithium_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    ds_handle_alice = MASQ_DS_init(LEVEL5, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen) */
    ds_handle_alice = MASQ_DS_qeep_init(LEVEL5);
#endif
    if(ds_handle_alice == NULL) {
        std::cout << "MASQ DS Handle init failed.\n";    
        return -1;    
    }
    std::cout << "Success!\n";


/** 
*   Set the Seed for random number generator
*/
    MASQ_DS_seed(ds_handle_bob, seed_orig, seed_len);
    MASQ_DS_seed(ds_handle_alice, seed_orig, seed_len);

    /* Create Alice's pk_alice & sk_alice keypair */
    std::cout << "Create Alice's keypair...\n";
    len_pk_alice = MASQ_DS_public_key_length(ds_handle_alice);
    len_sk_alice = MASQ_DS_private_key_length(ds_handle_alice);
    pk_alice = new uint8_t[len_pk_alice];
    sk_alice = new uint8_t[len_sk_alice];

    /* Setup Alice's signature */
    len_signature = MASQ_DS_signature_length(ds_handle_alice);

    signature = new uint8_t[len_signature];
   
/* Generate key pair*/
    MASQ_DS_keypair(ds_handle_alice, pk_alice, sk_alice);  
    std::cout << "Success!\n";


/* Alice: Uses own private key to sign the message */
    std::cout << "Alice uses own private key to sign the message...\n";
    MASQ_DS_sign(ds_handle_alice, sk_alice, msg, len_msg, signature, &len_signature);
    std::cout << "Success!\n";

 /** Send Alice's public key and signature to Bob **/
    sendPubKey();

/* Bob: Uses Alice's public key and verifies message and signature */
    std::cout << "Bob uses Alice's public key to verify the message and signature...\n";
    if(MASQ_DS_verify(ds_handle_bob, pk_alice, msg, len_msg, signature, len_signature) == 0)
    {
        std::cout << "Signature Verification Passed.\n";
    }
    else {
        std::cout << "Signature not Verified.\n";
    }
 
/** 
*   Free MASQ DS Handle
*/
#ifndef USE_PQRND
    MASQ_DS_free(ds_handle_alice);
    MASQ_DS_free(ds_handle_bob);
#else
    MASQ_DS_qeep_free(ds_handle_alice);
    MASQ_DS_qeep_free(ds_handle_bob);
#endif
   
    delete[] pk_alice;
    delete[] sk_alice;
    delete[] signature;
    delete[] msg;

    std::cout << "\n--------------------------------------------------------\n\n";
    return 0;
}