package com.example.demoqispaceapp

class masqDsKotlinWrapper {
    external fun masqDsQeepInit(level: Int): Long
    external fun masqDsSeed(handle: Long, seed: ByteArray, seedLen: Int): Int
    external fun masqDsPublicKeyLength(handle: Long): Int
    external fun masqDsPrivateKeyLength(handle: Long): Int
    external fun masqDsSignatureLength(handle: Long): Int
    external fun masqDsKeypair(handle: Long, publicKey: ByteArray, privateKey: ByteArray): Int
    external fun masqDsSign(handle: Long, privateKey: ByteArray, message: ByteArray, messageLen: Int,
                            signature: ByteArray, signatureLen: Int): Int
    external fun masqDsVerify(handle: Long, publicKey: ByteArray, message: ByteArray, messageLen: Int,
                              signature: ByteArray, signatureLen: Int): Int
    external fun masqDsQeepFree(handle: Long)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}