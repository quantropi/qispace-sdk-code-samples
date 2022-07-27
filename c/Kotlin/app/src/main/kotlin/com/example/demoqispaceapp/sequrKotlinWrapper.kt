package com.example.demoqispaceapp

class sequrKotlinWrapper {
    /**
     * Decodes an arbitrary QiSpace REST payload by using a cryptographic key and IV
     * @param key the cyptographic key, often the subkey, used to decode any payload from Enterprise REST API
     * @param iv an initialization vector applied during SEQUR decode process
     * @param encrypted_message the pre-encrypted data, usually from QiSpace REST API
     * @param decrypted_message the resulting data of the SEQUR decoding process
     * @return a status code indicating the state of the SEQUR decode
     */
    fun decode(key: ByteArray, iv: ByteArray, encrypted_message: ByteArray, decrypted_message: ByteArray): Int {

        // Return value used for debugging: see error.h
        var ret: Int

        // Step 1: create a handle
        val myHandle: Long = QSCinit()

        // Step 2: load the key
        ret = QSCqeepKeyLoad(myHandle, key, key.size)
        if (ret != 1) { return ret }

        // Step 3: set iv
        ret = QSCqeepIvSet(myHandle, iv, iv.size)
        if (ret != 1) { return ret }

        // Step 4: decode the message
        ret = QSCqeepDecode(myHandle, encrypted_message, encrypted_message.size, decrypted_message)
        if (ret != 1) { return ret }

        // Step 5: close the handle
        ret = QSCclose(myHandle)
        if (ret != 1) { return ret }

        return 1
    }

    // pQrnd: Generate random number
    // randSeed should be the QE
    /**
     * Performs the preparation for the PQRND API
     * @param randSeed the initial seed value for the Pseudo-Quantum Random Data generation
     * @return a status code indicating how the PQRND preparation went
     */
    fun pQrnd_setup(randSeed: ByteArray): Long {
        val ret: Int

        //Create the handle
        val randHandle = pQrndInitJ()

        //Set the seed for given handle
        ret = pQrndSeedLoadJ(randHandle, randSeed, randSeed.size)
        if(ret!=1)  return ret.toLong()
        else    return randHandle
    }

    /**
     * Generate pseudo quantum random data
     * @param randHandle a numeric value associated with the PQRND API
     * @param random_num the resulting pseudo quantum random data
     * @return the status of the PQRND API
     */
    fun pQrnd_generate(randHandle: Long, random_num: ByteArray): Int {
        return pQrndRndJ(randHandle, random_num, random_num.size)
    }

    /**
     * Clean up the PQRND API
     * @param randHandle a necessary numeric value associated with the PQRND API
     * @return a status code describing the cleanup process
     */
    fun pQrnd_close(randHandle: Long): Int {
        return pQrndCloseJ(randHandle)
    }

    // Let JNI load the QEEP SDK APIs from the native library
    private external fun QSCinit(): Long
    private external fun QSCqeepKeyLoad(handle: Long, qk: ByteArray, input_qk_len: Int): Int
    private external fun QSCqeepIvSet(handle: Long, iv: ByteArray, iv_len: Int): Int
    private external fun QSCqeepDecode(handle: Long, input: ByteArray, in_len: Int, out: ByteArray): Int
    private external fun QSCclose(handle: Long): Int

    // Load the pQrnd SDK APIs from the native library
    private external fun pQrndInitJ(): Long
    private external fun pQrndSeedLoadJ(handle: Long, seed: ByteArray, seed_len: Int): Int
    private external fun pQrndRndJ(handle: Long, rand: ByteArray, rand_len: Int): Int
    private external fun pQrndCloseJ(handle: Long): Int

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}