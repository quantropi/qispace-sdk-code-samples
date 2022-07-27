//
//  qeepSwiftWrapper.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-05-31.
//

import Foundation



class qeepSwiftWrapper{
    
    /// Definition of the wrapper class that calls QiSpace QEEP API
    /// Relevant header file: qeep.h
    
    /// Encrypts the message parameter using the key (most likely QK) with an IV
    ///
    /// - Parameters:
    ///  - key: a cryptographic key used for QEEP symmetric encryption
    ///  - iv: initialization vector for repetition reduction
    ///  - message: the message to be encrypted with QEEP
    ///  - encrypted_message: the result of QEEP encryption on the message using the key and IV
    /// - Returns: a status code describing the result of QEEP encryption procedure
    func encrypt(key qk: inout [UInt8], iv :inout [UInt8], message msg : inout [UInt8], encrypted_message enc_msg: inout [UInt8]) -> Int32{
        var myHandle : QP_Handle?
        
        let qk_len = Int32(qk.count)
        let iv_len = Int32(iv.count)
        let msg_len = Int32(msg.count)
        
    
        // QEEP_RET defined in error.h (used for debugging)
        // Step 1: Initialize handle
        var ret: QEEP_RET = QP_init(&myHandle)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 2: Load QEEP key (qk)
        ret = QP_qeep_key_load(myHandle, &qk, qk_len)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 3: Set iv
        ret = QP_iv_set(myHandle, &iv, iv_len)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 4: Encrypt the message
        ret = QP_encrypt(myHandle, &msg, msg_len, &enc_msg)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 5: Close the handle
        ret = QP_close(myHandle)
        if ret != QEEP_OK { return ret.rawValue }

        return 1
    }
    
    /// Decrypts the encrypted_message using the symmetric key (QK) and IV
    ///
    /// - Parameters:
    ///  - key: symmetric cryptographic key for QEEP decryption
    ///  - iv: initialization vectorfor repetition reduction
    ///  - encrypted_message: the previously QEEP encrypted message intended for decryption
    ///  - message: the result of the QEEP decryption 
    /// - Returns: a status code describing the result of QEEP decryption of the message
    func decrypt(key qk: inout [UInt8], iv: inout [UInt8], encrypted_message enc_msg: inout [UInt8], message msg: inout [UInt8]) -> Int32 {
        var myHandle: QP_Handle?
        
        let qk_len = Int32(qk.count)
        let msg_len = Int32(enc_msg.count)
        let iv_len = Int32(iv.count)
        
        // Step 1: Initialize handle
        var ret = QP_init(&myHandle)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 2: Load QEEP key (qk)
        ret = QP_qeep_key_load(myHandle, &qk, qk_len)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 3: Set iv
        ret = QP_iv_set(myHandle, &iv, iv_len)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 4: Decrypt the message
        ret = QP_decrypt(myHandle, &enc_msg, msg_len, &msg)
        if ret != QEEP_OK { return ret.rawValue }
        
        // Step 5: Close the handle
        ret = QP_close(myHandle)
        if ret != QEEP_OK { return ret.rawValue }
        
        return 1
    }
}
