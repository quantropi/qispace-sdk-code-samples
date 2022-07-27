package com.example.demoqispaceapp

class qeepKotlinWrapper {
    /**
     * Produces an encrypted message, as a byte array, based on the key and IV
     * @param key the symmetric cryptographic key used for encryption and decryption
     * @param iv an initialization vector used during the QEEP encryption process
     * @param message the message desired to be encrypted
     * @param encrypted_message the result of performing QEEP encrypt on the message
     * @return the status of the encrypt operation performed on the message
     */
    fun encrypt(key: ByteArray, iv: ByteArray, message: ByteArray, encrypted_message: ByteArray): Int {

        // Return value used for debugging: see error.h
        var ret: Int

        // Step 1: create a handle
        val myHandle: Long = QPinit()

        // Step 2: load the key
        ret = QPkeyLoad(myHandle, key, key.size)
        if (ret != 1) { return ret }

        // Step 3: set iv
        ret = QPivSet(myHandle, iv, iv.size)
        if (ret != 1) { return ret }

        // Step 4: encode the message
        ret = QPencrypt(myHandle, message, message.size, encrypted_message)
        if (ret != 1) { return ret }

        // Step 5: close the handle
        ret = QPclose(myHandle)
        if (ret != 1) { return ret }

        return 1
    }

    /**
     * Decrypts the encrypted message by using the same key and IV
     * @param key the symmetric cryptographic key used for both encryption and decryption
     * @param iv an initialization vector used for encryption and decryption
     * @param encrypted_message the QEEP-encrypted message to be decrypted
     * @param decrypted_message the result of decrypting the QEEP-encrypted message
     * @return the status of the decrypt operation performed on the message
     */
    fun decrypt(key: ByteArray, iv: ByteArray, encrypted_message: ByteArray, decrypted_message: ByteArray): Int {

        // Return value used for debugging: see error.h
        var ret: Int

        // Step 1: create a handle
        val myHandle: Long = QPinit()

        // Step 2: load the key
        ret = QPkeyLoad(myHandle, key, key.size)
        if (ret != 1) { return ret }

        // Step 3: set iv
        ret = QPivSet(myHandle, iv, iv.size)
        if (ret != 1) { return ret }

        // Step 4: decode the message
        ret = QPdecrypt(myHandle, encrypted_message, encrypted_message.size, decrypted_message)
        if (ret != 1) { return ret }

        // Step 5: close the handle
        ret = QPclose(myHandle)
        if (ret != 1) { return ret }

        return 1
    }

    // Let JNI load the QEEP SDK APIs from the native library
    private external fun QPinit(): Long
    private external fun QPkeyLoad(handle: Long, qk: ByteArray, input_qk_len: Int): Int
    private external fun QPivSet(handle: Long, iv: ByteArray, iv_len: Int): Int
    private external fun QPencrypt(handle: Long, input: ByteArray, in_len: Int, out: ByteArray): Int
    private external fun QPdecrypt(handle: Long, input: ByteArray, in_len: Int, out: ByteArray): Int
    private external fun QPclose(handle: Long): Int

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}