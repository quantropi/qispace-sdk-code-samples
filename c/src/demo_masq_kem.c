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
#include "../lib/masq_kem.h"
#include "../lib/qispace_pqrnd.h"

/* 
 * This demo displays one example of the MASQ KEM API, specifically HPPK level 3, 
 * to establish a shared secret between two parties (Alice and Bob)
 *
 * In this case, Alice, who holds a shared secret, desires to let Bob know her shared secret. 
 * So, Bob begins by generating a public-private keypair and sends Alice the public key. 
 * Alice then uses the public key to encapsulate her shared secret,
 * and sends this encapsulated data back to Bob.
 * With his previously generated private key, Bob proceeds to decapsulate this data and thus, 
 * both Alice and Bob arrive at the same shared secret.
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

/* Initialize/set seed for any random number generator used by Alice and Bob
* We recommand to call QiSpace API (i.e. QE) to get seed 
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
    printf("\n*Bob sends Alice his public key.*\n");
}

void sendCipherTxt(){
    printf("\n*Alice sends Bob her encapsulated shared secret.*\n");
}

int main(int argc, const char * argv[]) {

    MASQ_KEM_handle *kem_handle_alice, *kem_handle_bob;
    int32_t len_pk_bob, len_sk_bob, len_ciphertext, len_ss;
    unsigned char *pk_bob, *sk_bob, *ciphertext, *ss_alice, *ss_bob;
    unsigned char alice_seed[1024], bob_seed[1024];
    int seed_len = 32;
    

    /* Init: Initialize MASQ KEM handles for Bob and Alice */
    printf("\nInitialize MASQ KEM handles for Bob and Alice...\n");
    
    /* Uses default Quantropi random callback function */    
    kem_handle_alice = MASQ_KEM_init(HPPKL3, NULL, NULL, NULL); 
    
    /* Uses provided random callback function */   
    //kem_handle_alice = MASQ_KEM_init(HPPKL3, rand_cf, rand_seed_cf, NULL);
    
    if(kem_handle_alice == NULL) {
        printf("\nMASQ KEM Handle init failed for Alice.\n");    
        return MASQ_ERR_MEM_Alloc;    
    }

    /* Uses default Quantropi random callback function */
    kem_handle_bob = MASQ_KEM_init(HPPKL3, NULL, NULL, NULL);
    
    /* Uses provided random callback function */   
    //kem_handle_bob = MASQ_KEM_init(HPPKL3, rand_cf, rand_seed_cf, NULL);
    
    if(kem_handle_bob == NULL) {
        printf("\nMASQ KEM Handle init failed for Bob.\n");    
        return MASQ_ERR_MEM_Alloc;    
    }
    printf("\nSuccess!\n");


    /* Set the seed for random number generator for Alice */
    gen_rand_seed_alice(alice_seed, seed_len);
    MASQ_KEM_seed(kem_handle_alice, alice_seed, seed_len);
    /** Set the Seed for random number generator for Bob*/
    gen_rand_seed_bob(bob_seed, seed_len);
    MASQ_KEM_seed(kem_handle_bob, bob_seed, seed_len);


    /* 1a. Setup Alice's shared secret */
    printf("\nSetup Alice's shared secret...\n");
    len_ss = MASQ_KEM_shared_secret_length(kem_handle_alice);
    ss_alice = malloc(len_ss);

    /* 1b.  Initialize Bob's future shared secret */
    ss_bob = malloc(len_ss);


    /* 2. Create Bob's pk_bob & sk_bob keypair */
    printf("\nCreate Bob's keypair...\n");
    len_pk_bob = MASQ_KEM_public_key_length(kem_handle_bob);
    len_sk_bob = MASQ_KEM_secret_key_length(kem_handle_bob);
    pk_bob = malloc(len_pk_bob);
    sk_bob = malloc(len_sk_bob);

    MASQ_KEM_keypair(kem_handle_bob, pk_bob, sk_bob);
    printf("\nSuccess!\n");


    /** Send Bob's public key to Alice **/
    sendPubKey();


    /* 3. Alice: Use Bob's public key to encrypt the shared secret into ciphertext */
    printf("\nAlice uses Bob's public key to encapsulate the shared secret...\n");
    len_ciphertext = MASQ_KEM_ciphertext_length(kem_handle_alice);
    ciphertext = malloc(len_ciphertext);
    MASQ_KEM_encaps(kem_handle_alice, pk_bob, ss_alice, ciphertext);
    printf("\nSuccess!\n");


    /** Send Alice's encrypted ciphertext to Bob */
    sendCipherTxt();


    /* 4. Bob: Use his own secret key to decrypt the shared secret into ciphertext */
    printf("\nBob decapsulate Alice's ciphertext using his own private key...\n");
    MASQ_KEM_decaps(kem_handle_bob, sk_bob, ciphertext, ss_bob);
    printf("\nSuccess!\n");


    /* Check: Verify Alice and Bob shared secrets from KEM match */
    printf("\nVerify shared secrets match...\n");
    if (memcmp(ss_alice, ss_bob, len_ss) != 0) {
        printf("%s\n", "\nError: Shared Secret do not match.\n");
    }
    else {
        printf("%s\n", "\nSuccess: Shared Seceret Matches.\n");
    }
    printf("The Shared Secret Bob received: [%X]\n", ss_bob);

    /* Free MASQ KEM data */
    MASQ_KEM_free(kem_handle_alice);
    MASQ_KEM_free(kem_handle_bob);
    free(pk_bob);
    free(sk_bob);
    free(ciphertext);
    free(ss_alice);
    free(ss_bob);

    printf("\n--------------------------------------------------------\n\n");
    
    return 0;
}