package com.example.qispace_sdk_java_demo;

// This is the wrapper class that uses the QEEP SDK APIs
public class qeepJavaWrapper {
    /**
     * Produces an encrypted message, as a byte array, based on the key and IV
     * @param key the symmetric cryptographic key used for encryption and decryption
     * @param iv an initialization vector used during the QEEP encryption process
     * @param message the message desired to be encrypted
     * @param encrypted_message the result of performing QEEP encrypt on the message
     * @return the status of the encrypt operation performed on the message
     */
    public int encrypt(byte[] key, byte[] iv, byte[] message, byte[] encrypted_message) {

        // Return value used for debugging: see error.h
        int ret;

        // Step 1: create a handle
        long myHandle = QPinit();

        // Step 2: load the key
        ret = QPkeyLoad(myHandle, key, key.length);
        if(ret != 1) { return ret; }

        // Step 3: set iv
        ret = QPivSet(myHandle, iv, iv.length);
        if(ret != 1) { return ret; }

        // Step 4: encode the message
        ret = QPencrypt(myHandle, message, message.length, encrypted_message);
        if(ret != 1) { return ret; }

        // Step 5: close the handle
        ret = QPclose(myHandle);
        if(ret != 1) { return ret; }

        return 1;
    }

    /**
     * Decrypts the encrypted message by using the same key and IV
     * @param key the symmetric cryptographic key used for both encryption and decryption
     * @param iv an initialization vector used for encryption and decryption
     * @param encrypted_message the QEEP-encrypted message to be decrypted
     * @param decrypted_message the result of decrypting the QEEP-encrypted message
     * @return the status of the decrypt operation performed on the message
     */
    public int decrypt(byte[] key, byte[] iv, byte[] encrypted_message, byte[] decrypted_message) {

        // Return value used for debugging: see error.h
        int ret;

        // Step 1: create a handle
        long myHandle = QPinit();

        // Step 2: load the key
        ret = QPkeyLoad(myHandle, key, key.length);
        if(ret != 1) { return ret; }

        // Step 3: set iv
        ret = QPivSet(myHandle, iv, iv.length);
        if(ret != 1) { return ret; }

        // Step 4: decode the encoded message
        ret = QPdecrypt(myHandle, encrypted_message, encrypted_message.length, decrypted_message);
        if(ret != 1) { return ret; }

        // Step 5: close the handle
        ret = QPclose(myHandle);
        if(ret != 1) { return ret; }

        return 1;
    }

    // Let JNI load the SEQUR SDK APIs from the native library
    private native long QPinit();
    private native int QPkeyLoad(long handle, byte[] qk, int qk_len);
    private native int QPivSet(long handle, byte[] iv, int iv_len);
    private native int QPencrypt(long handle, byte[] msg, int msg_len, byte[] encrypted_msg);
    private native int QPdecrypt(long handle, byte[] encrypted_msg, int msg_len, byte[] decrypted_message);
    private native int QPclose(long handle);

    // Loading the JNI wrapper native library
    static {
        System.loadLibrary("native-lib");
    }
}
