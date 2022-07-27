//
//  qeepObjCWrapper.m
//  QEEPobjectiveC
//
//  Created by Roy Zheng on 2022-06-08.
//

#import <Foundation/Foundation.h>
#import "../QiSpace_SDK_Lib/qispace_qeep.h"
#import "../QiSpace_SDK_Lib/error.h"

int QEEP_encrypt(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *msg, int32_t msg_len, uint8_t * encoded_msg){
    //QEEP_RET defined in error.h (used for debugging)
    QEEP_RET ret;
    
    QP_Handle myHandle;
    
    //Step 1: Initialize handle
    ret = QP_init(&myHandle);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 2: Load QEEP key (qk)
    ret = QP_qeep_key_load(myHandle, qk, qk_len);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 3: Set iv
    ret = QP_iv_set(myHandle, iv, 16);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 4: Encode the message
    ret = QP_encrypt(myHandle, msg, msg_len, encoded_msg);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 5: Close the handle
    ret = QP_close(myHandle);
    if (ret != QEEP_OK) { return ret; }
    
    return QEEP_OK;
}

int QEEP_decrypt(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *encoded_msg, int32_t encoded_msg_len, uint8_t * decoded_msg) {
    //QEEP_RET defined in error.h (used for debugging)
    QEEP_RET ret;
    
    QP_Handle myHandle;
    
    //Step 1: Initialize handle
    ret = QP_init(&myHandle);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 2: Load QEEP key (qk)
    ret = QP_qeep_key_load(myHandle, qk, qk_len);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 3: Set iv
    ret = QP_iv_set(myHandle, iv, 16);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 4: Encode the message
    ret = QP_decrypt(myHandle, encoded_msg, encoded_msg_len, decoded_msg);
    if (ret != QEEP_OK) { return ret; }
    
    //Step 5: Close the handle
    ret = QP_close(myHandle);
    if (ret != QEEP_OK) { return ret; }
    
    return QEEP_OK;
}

