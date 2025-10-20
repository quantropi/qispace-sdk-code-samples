package com.example.demoqispaceapp

class masqKemKotlinWrapper {
    external fun masqKemQeepInit(level: Int): Long
    external fun masqKemUseKeyFormat(handle: Long): Int
    external fun masqKemSeed(handle: Long, seed: ByteArray, seedLen: Int): Int
    external fun masqKemPublicKeyLength(handle: Long): Int
    external fun masqKemSecretKeyLength(handle: Long): Int
    external fun masqKemCiphertextLength(handle: Long): Int
    external fun masqKemSharedSecretLength(handle: Long): Int
    external fun masqKemKeypair(handle: Long, publicKey: ByteArray, secretKey: ByteArray): Int
    external fun masqKemEncaps(handle: Long, publicKey: ByteArray, sharedSecret: ByteArray, ciphertext: ByteArray): Int
    external fun masqKemDecaps(handle: Long, secretKey: ByteArray, ciphertext: ByteArray, sharedSecret: ByteArray): Int
    external fun masqKemQeepFree(handle: Long)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}