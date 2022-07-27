//
//  sequrObjCWrapper.m
//  QEEPobjectiveC
//
//  Created by Roy Zheng on 2022-06-08.
//

#import <Foundation/Foundation.h>
#import "../QiSpace_SDK_Lib/qispace_sequr.h"
#import "../QiSpace_SDK_Lib/qispace_pqrnd.h"
#import "../QiSpace_SDK_Lib/error.h"

int SEQUR_decode(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *encoded_msg, int32_t encoded_msg_len, uint8_t * decoded_msg) {
    
    //QEEP_RET defined in error.h (used for debugging)
    QEEP_RET ret;
    QSC_Handle myHandle;
    
    //Step 1: Initialize handle
    ret = QSC_init(&myHandle);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 2: Load QEEP key (qk)
    ret = QSC_qeep_key_load(myHandle, qk, qk_len);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 3: Set iv
    ret = QSC_qeep_iv_set(myHandle, iv, 16);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 4: Decode the message
    ret = QSC_qeep_decode(myHandle, encoded_msg, encoded_msg_len, decoded_msg);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 5: Close the handle
    ret = QSC_close(myHandle);
    if (ret != QEEP_OK) { return ret; }
    return QEEP_OK;
}

int pQrnd_setup(uint8_t *randSeed, int randSeed_len, pQrnd_Handle *randHandle){
    QEEP_RET ret;
    
    //Create the handle
    ret = pQrndInit(randHandle);
    
    //Set the seed for given handle
    ret = pQrndSeed(*randHandle, randSeed, randSeed_len);
    
    return ret;
}

//Handles the random generation (separated from setup and close to enable per-button press generation)
int pQrnd_generate(pQrnd_Handle randHandle, uint8_t *rand, int rand_len){
    QEEP_RET ret;
    ret = pQrndRnd(randHandle, rand, rand_len);
    return ret;
}

int pQrnd_close(pQrnd_Handle randHandle){
    return pQrndClose(randHandle);
}
