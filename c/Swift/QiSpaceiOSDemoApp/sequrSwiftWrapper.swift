//
//  sequrSwiftWrapper.swift
//  QEEPiOSDemoApp
//
//  Created by Roy Zheng on 2022-05-31.
//

import Foundation
class sequrSwiftWrapper{
    
    /// Definition of the wrapper class that calls the SEQUR API functions
    /// Relevant header file: sequr.h
    
    /// SEQUR: decoding payload (QK, QE and their IVs for this demo)
    ///
    /// - Parameters:
    ///  - key: the cryptographic key used to perform the SEQUR 'decoding'
    ///  - iv: an initialization vector used as noise for reducing repetition during decryption
    ///  - encoded_message: the payload to be decoded
    ///  - decoded_message: the result of the decoded payload
    /// - Returns an integer status code describing what happened during decoding process
    func decode(key qe: inout [UInt8], iv: inout [UInt8], encoded_message enc_msg: inout [UInt8], decoded_message dec_msg: inout [UInt8]) -> Int32 {
        var myHandle: QSC_Handle?
        
        let qe_len = Int32(qe.count)
        let msg_len = Int32(enc_msg.count)
        let iv_len = Int32(iv.count)
        
        // QEEP_RET defined in error.h (used for debugging)
        // Step 1: Initialize handle
        var ret = QSC_init(&myHandle)
        if ret != QEEP_OK {
            print("QSCinit failed")
            return ret.rawValue
        }
        
        // Step 2: Load Quantum Entropy (qe)
        ret = QSC_qeep_key_load(myHandle, &qe, qe_len)
        if ret != QEEP_OK {
            print("QSCkeyLoad failed")
            return ret.rawValue
        }
        // Step 3: Set iv
        ret = QSC_qeep_iv_set(myHandle, &iv, iv_len)
        if ret != QEEP_OK {
            print("QSCivSet failed")
            return ret.rawValue
        }
        
        // Step 4: Decrypt the message
        ret = QSC_qeep_decode(myHandle, &enc_msg, msg_len, &dec_msg)
        if ret != QEEP_OK {
            print("QSCdecrypt failed")
            return ret.rawValue
        }
        
        // Step 5: Close the handle
        ret = QSC_close(myHandle)
        if ret != QEEP_OK {
            print("QSCclose failed")
            return ret.rawValue
        }
        
        return 1
    }
    
    /// pQrnd: Generates random number
    /// Start setup (randSeed will be the QE for this demo)
    /// Prepares the PQRND API for pseudo quantum random generation
    ///
    /// - Parameters:
    ///  - seed: the initial datum used to seed the PQRND API
    ///  - handle: a numeric value associated with the PQRND API
    /// - Returns: a status code describing the execution of initializing and seeding PQRND
    func pQrnd_setup(seed randSeed: inout[UInt8], handle myHandle : inout pQrnd_Handle?) -> (QEEP_RET){

        var ret : QEEP_RET
        let seed_len = Int32(randSeed.count)

        // Initialize the handle
        ret = pQrndInit(&myHandle)

        // Set the seed for given handle
        ret = pQrndSeed(myHandle, &randSeed, seed_len)
        return ret.self
    }

    /// Generate pseudo quantum random
    ///
    /// - Parameters:
    ///  - handle: a numeric value associated with the PQRND API
    ///  - random_number_to_be: the result of PQRND API pseudo-quantum random generation
    /// - Returns: a status code describing the result of the random data generation
    func pQrnd_generate(handle randHandle: pQrnd_Handle, random_number_to_be random_num : inout[UInt8]) -> QEEP_RET{
        let rand_len = Int32(random_num.count)

        let ret = pQrndRnd(randHandle, &random_num, rand_len)

        return ret;
    }
    
    /// Close and cleanup
    ///
    /// - Parameters:
    ///  - handle:same numeric value associated with the PQRND API
    /// - Returns: the status code of attempting to close PQRND API
    func pQrnd_close_handle(handle randHandle: pQrnd_Handle) -> QEEP_RET{
        return pQrndClose(randHandle)
    }
    
}
