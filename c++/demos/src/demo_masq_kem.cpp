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
#include "masq_kem.h"
#ifdef USE_KYBER_RANDOM
#include "kyber_randombytes.h"
#endif

using namespace std;

/* 
 * This demo displays one example of the MASQ KEM API, specifically HPPK level 3, to establish a shared secret between two parties (Alice and Bob)
 *
 * In this case, Alice, who holds a shared secret, desires to let Bob know her shared secret. So, Bob begins by generating a public-private keypair and
 * sends Alice the public key. Alice then uses the public key to encapsulate her shared secret, and sends this encapsulated data back to Bob.
 * With his previously generated private key, Bob proceeds to decapsulate this data and thus, both Alice and Bob arrive at the same shared secret.
 */

unsigned char seed_orig[1024];
const int32_t seed_len = 32; 


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
    
    if (rand_num == nullptr) return 0;
    
    if (rand_handle == nullptr) 
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

void sendPubKey(){
    std::cout << "*Bob sends Alice his public key...*\n";
}

void sendCipherTxt(){
    std::cout << "*Alice sends Bob her encapsulated shared secret...*\n";
}

int main(int argc, const char * argv[]) {

    MASQ_KEM_handle *kem_handle_alice, *kem_handle_bob;
    int32_t len_pk_bob, len_sk_bob, len_ciphertext, len_ss;
    unsigned char *pk_bob, *sk_bob, *ciphertext, *ss_alice, *ss_bob;

   /* Initialize random seed */ 
    seed_init();

    /* Init: Initialize MASQ KEM handles for Bob and Alice */

    std::cout << "Initialize MASQ KEM handles for Bob and Alice...\n";

#ifndef USE_PQRND
#ifdef USE_KYBER_RANDOM
    /* Uses Kyber default random generator */
    kem_handle_alice = MASQ_KEM_init(LEVEL3, (MASQ_rand_callback_t)kyber_rand_cf, (MASQ_rand_seed_callback_t)kyber_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    kem_handle_alice = MASQ_KEM_init(LEVEL3, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen)     */
    kem_handle_alice = MASQ_KEM_qeep_init(LEVEL3); 
#endif

    if(kem_handle_alice == nullptr) {
        std::cout << "MASQ KEM Handle init failed.\n";    
        return -1;    
    }

#ifndef USE_PQRND
#ifdef USE_KYBER_RANDOM
    /* Uses Kyber default random generator */
    kem_handle_bob = MASQ_KEM_init(LEVEL3, (MASQ_rand_callback_t)kyber_rand_cf, (MASQ_rand_seed_callback_t)kyber_rand_seed_cf, NULL);
#else
    /* Uses Demo Provided Random Generator */
    kem_handle_bob = MASQ_KEM_init(LEVEL3, rand_cf, rand_seed_cf, NULL);
#endif
#else
    /* Uses Quantropi default random generator (i.e. SEQUR NGen)     */
    kem_handle_bob = MASQ_KEM_qeep_init(LEVEL3);
#endif

    if(kem_handle_bob == NULL) {
        std::cout << "MASQ KEM Handle init failed.\n";    
        return -1;    
    }
    std::cout << "Success!\n";


    /* Set the seed for random number generator */
    MASQ_KEM_seed(kem_handle_alice, seed_orig, seed_len);
    MASQ_KEM_seed(kem_handle_bob, seed_orig, seed_len);


    /* 1a. Setup Alice's shared secret */
    std::cout << "Setup Alice's shared secret...\n";
    len_ss = MASQ_KEM_shared_secret_length(kem_handle_alice);
    ss_alice = new uint8_t[len_ss];

    /* Here we are randomly generating the shared secret */
    for (int i = 0; i < len_ss; i++)
    {
        ss_alice[i] = rand() & 0xff;
    }
    std::cout << "Success!\n";

    /* 1b.  Initialize Bob's future shared secret */
    ss_bob = new uint8_t[len_ss];


    /* 2. Create Bob's public key and private key*/
    std::cout << "Create Bob's keypair...\n";
    len_pk_bob = MASQ_KEM_public_key_length(kem_handle_bob);
    len_sk_bob = MASQ_KEM_secret_key_length(kem_handle_bob);
    pk_bob = new uint8_t[len_pk_bob];
    sk_bob = new uint8_t[len_sk_bob];

    /* Generate key pair for Bob*/
    MASQ_KEM_keypair(kem_handle_bob, pk_bob, sk_bob);
    std::cout << "Success!\n";


    /** Send Bob's public key to Alice **/
    sendPubKey();


    /* 3. Alice: Uses Bob's public key to encrypt the shared secret into ciphertext */
    std::cout << "Alice uses Bob's public key to encapsulate the shared secret...\n";
    len_ciphertext = MASQ_KEM_ciphertext_length(kem_handle_alice);
    ciphertext = new uint8_t[len_ciphertext];

    /*  Encapsulate Alice's shared secret */
    MASQ_KEM_encaps(kem_handle_alice, pk_bob, ss_alice, ciphertext);
    std::cout << "Success!\n";


    /** Send Alice's encrypted ciphertext to Bob */
    sendCipherTxt();


    /* 4. Bob: Use his own secret key to decrypt the shared secret into ciphertext */
    std::cout << "Bob decapsulate Alice's ciphertext...\n";
    MASQ_KEM_decaps(kem_handle_bob, sk_bob, ciphertext, ss_bob);
    std::cout << "Success!\n";


    /* Check: Verify Alice and Bob shared secrets from KEM match */
    std::cout << "Verify shared secrets match...\n";
    if (memcmp(ss_alice, ss_bob, len_ss) != 0) {
        std::cout << "Error: Shared Secret do not match.\n";
    }
    else {
        std::cout << "Success: Shared Secret Matches.\n";
    }
    

    /* Free MASQ KEM data */
#ifndef USE_PQRND
    MASQ_KEM_free(kem_handle_alice);
    MASQ_KEM_free(kem_handle_bob);
#else
    MASQ_KEM_qeep_free(kem_handle_alice);
    MASQ_KEM_qeep_free(kem_handle_bob);
#endif
    delete[] pk_bob;
    delete[] sk_bob;
    delete[] ciphertext;
    delete[] ss_alice;
    delete[] ss_bob;

    std::cout << "\n--------------------------------------------------------\n\n";
    
    return 0;
}