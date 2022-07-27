package com.example.qispace_sdk_java_demo;

public class sequrJavaWrapper {
    /**
     * Decodes an arbitrary QiSpace REST payload by using a cryptographic key and IV
     * @param key the cyptographic key, often the subkey, used to decode any payload from Enterprise REST API
     * @param iv an initialization vector applied during SEQUR decode process
     * @param encrypted_message the pre-encrypted data, usually from QiSpace REST API
     * @param decrypted_message the resulting data of the SEQUR decoding process
     * @return a status code indicating the state of the SEQUR decode
     */
    public int decode(byte[] key, byte[] iv, byte[] encrypted_message, byte[] decrypted_message) {

        // Return value used for debugging: see error.h
        int ret;

        // Step 1: create a handle
        long myHandle = QSCinit();

        // Step 2: load the key
        ret = QSCKeyLoad(myHandle, key, key.length);
        if(ret != 1) { return ret; }

        // Step 3: set iv
        ret = QSCIvSet(myHandle, iv, iv.length);
        if(ret != 1) { return ret; }

        // Step 4: decode the encoded message
        ret = QSCDecode(myHandle, encrypted_message, encrypted_message.length, decrypted_message);
        if(ret != 1) { return ret; }

        // Step 5: close the handle
        ret = QSCclose(myHandle);
        if(ret != 1) { return ret; }

        return 1;
    }

    // pQrnd: Generate random number
    // randSeed should be the QE

    /**
     * Performs the preparation for the PQRND API
     * @param randSeed the initial seed value for the Pseudo-Quantum Random Data generation
     * @return a status code indicating how the PQRND preparation went
     */
    public long pQrnd_setup(byte[] randSeed){
        int ret;

        // Create the handle
        long randHandle = pQrndInitJ();

        // Set the seed for given handle
        ret = pQrndSeedLoadJ(randHandle, randSeed, randSeed.length);
        if(ret != 1)    return ret;

        return randHandle;
    }

    /**
     * Generate pseudo quantum random data
     * @param randHandle a numeric value associated with the PQRND API
     * @param random_num the resulting pseudo quantum random data
     * @return the status of the PQRND API
     */
    public int pQrnd_generate(long randHandle, byte[] random_num){
        int ret = pQrndRndJ(randHandle, random_num, random_num.length);
        return ret;
    }

    /**
     * Clean up the PQRND API
     * @param randHandle a necessary numeric value associated with the PQRND API
     * @return a status code describing the cleanup process
     */
    public int pQrnd_close(long randHandle){
        int ret = pQrndCloseJ(randHandle);
        return ret;
    }

    // Load the SEQUR SDK APIs from the native library
    private native long QSCinit();
    private native int QSCKeyLoad(long handle, byte[] qk, int qk_len);
    private native int QSCIvSet(long handle, byte[] iv, int iv_len);
    private native int QSCDecode(long handle, byte[] encrypted_msg, int msg_len, byte[] decrypted_message);
    private native int QSCclose(long handle);

    // Load the pQrnd SDK APIs from the native library
    private native long pQrndInitJ();
    private native int pQrndSeedLoadJ(long handle, byte[] seed, int seed_len);
    private native int pQrndRndJ(long handle, byte[] rand, int rand_len);
    private native int pQrndCloseJ(long handle);

    // Loading the JNI wrapper native library
    static {
        System.loadLibrary("native-lib");
    }
}
