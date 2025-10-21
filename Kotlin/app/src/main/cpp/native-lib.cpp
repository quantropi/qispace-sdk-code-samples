#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "QiSpaceNative"

// Path is based on current directory
// Add QEEP includes
#include "../../../../QiSpace_SDK_Lib/qeep/lib/qispace_pqrnd.h"
#include "../../../../QiSpace_SDK_Lib/qeep/lib/qispace_qeep.h"
#include "../../../../QiSpace_SDK_Lib/qeep/lib/qispace_sequr.h"
#include "../../../../QiSpace_SDK_Lib/qeep/lib/error.h"
// Add MASQ includes
#define MASQ_KEM_hppk
#define MASQ_DS_ghppk
#include "../../../../QiSpace_SDK_Lib/kem/lib_hppk/masq_kem.h"
#include "../../../../QiSpace_SDK_Lib/kem/lib_hppk/masq_kem_error.h"
#include "../../../../QiSpace_SDK_Lib/ds/lib_ghppk/masq_ds.h"
#include "../../../../QiSpace_SDK_Lib/ds/lib_ghppk/masq_ds_error.h"


// This file calls the QEEP SDK APIs
// It maps each function mentioned in sequr.h and qeep.h to a JNI function,
// which can be called in kotlin code

// The function name must include the path of the class accessing it
// Each directory is separated by a "_"
// There is a "1" in the path because there is a "_" character in the folder name
// The "1" tells JNI to treat "_" as a normal character, not new directory
// Finally, the name of the function to be used is inserted at the end of the path
// Here, it is QSCinit, so when used in kotlin, we call QSCinit()


//------------------------------------------------------------------------------
//Wrapping QEEP APIs:

extern "C"
JNIEXPORT jlong
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPinit(
        JNIEnv* env,
        jobject ) {
    QEEP_RET ret;
    QP_Handle qp_handle;
    ret = QP_init(&qp_handle);
    if (ret != QEEP_OK)  qp_handle = 0;
    return (unsigned long)(qp_handle);
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPkeyLoad(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray qk,
        jint input_qk_len) {
    QEEP_RET ret;
    jbyte *jqk = (*env).GetByteArrayElements(qk, 0);
    ret = QP_qeep_key_load((QP_Handle)(unsigned long)handle, (uint8_t*)jqk, input_qk_len);
    (*env).ReleaseByteArrayElements(qk, jqk, 0);
    return (int)ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPivSet(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray iv,
        jint iv_len) {
    QEEP_RET ret;
    jbyte *jiv = (*env).GetByteArrayElements(iv, 0);
    ret = QP_iv_set((QP_Handle)(unsigned long)handle, (uint8_t*)jiv, iv_len);
    return (int)ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPencrypt(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray input,
        jint in_len,
        jbyteArray out) {
    QEEP_RET ret;
    jbyte *jin = (*env).GetByteArrayElements(input, 0);
    jbyte *jout = (*env).GetByteArrayElements(out, 0);
    ret = QP_encrypt((QP_Handle)(unsigned long)handle, (uint8_t*)jin, in_len, (uint8_t*)jout);
    (*env).ReleaseByteArrayElements(input, jin, 0);
    (*env).ReleaseByteArrayElements(out, jout, 0);
    return (int)ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPdecrypt(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray input,
        jint in_len,
        jbyteArray out) {
    QEEP_RET ret;
    jbyte *jin = (*env).GetByteArrayElements(input, 0);
    jbyte *jout = (*env).GetByteArrayElements(out, 0);
    ret = QP_decrypt((QP_Handle) (unsigned long) handle, (uint8_t *) jin, in_len,
                     (uint8_t *) jout);
    (*env).ReleaseByteArrayElements(input, jin, 0);
    (*env).ReleaseByteArrayElements(out, jout, 0);
    return (int) ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_qeepKotlinWrapper_QPclose(
        JNIEnv* env,
        jobject /* this */,
        jlong handle) {
    QEEP_RET ret;
    ret = QP_close((QP_Handle)(unsigned long)handle);
    return (int)ret;
}

//-------------------------------------------------------------------------------------------
//  Wrapping SEQUR APIs

extern "C"
JNIEXPORT jlong JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_QSCinit(JNIEnv *env, jobject thiz) {
    QEEP_RET ret;
    QSC_Handle qsc_handle;
    ret = QSC_init(&qsc_handle);
    if (ret != QEEP_OK)  qsc_handle = 0;
    return (unsigned long)(qsc_handle);
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_QSCqeepKeyLoad(JNIEnv *env, jobject thiz,
                                                             jlong handle, jbyteArray qk,
                                                             jint input_qk_len) {
    QEEP_RET ret;
    jbyte *jqk = (*env).GetByteArrayElements(qk, 0);
    ret = QSC_qeep_key_load((QSC_Handle)(unsigned long)handle, (uint8_t*)jqk, input_qk_len);
    (*env).ReleaseByteArrayElements(qk, jqk, 0);
    return (int)ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_QSCqeepIvSet(JNIEnv *env, jobject thiz, jlong handle,
                                                           jbyteArray iv, jint iv_len) {
    QEEP_RET ret;
    jbyte *jiv = (*env).GetByteArrayElements(iv, 0);
    ret = QSC_qeep_iv_set((QSC_Handle)(unsigned long)handle, (uint8_t*)jiv, iv_len);
    return (int)ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_QSCqeepDecode(JNIEnv *env, jobject thiz, jlong handle,
                                                            jbyteArray input, jint in_len,
                                                            jbyteArray out) {
    QEEP_RET ret;
    jbyte *jin = (*env).GetByteArrayElements(input, 0);
    jbyte *jout = (*env).GetByteArrayElements(out, 0);
    ret = QSC_qeep_decode((QSC_Handle) (unsigned long) handle, (uint8_t *) jin, in_len,
                          (uint8_t *) jout);
    (*env).ReleaseByteArrayElements(input, jin, 0);
    (*env).ReleaseByteArrayElements(out, jout, 0);
    return (int) ret;
}

extern "C"
JNIEXPORT jint JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_QSCclose(JNIEnv *env, jobject thiz, jlong handle) {
    QEEP_RET ret;
    ret = QSC_close((QSC_Handle)(unsigned long)handle);
    return (int)ret;
}

//-----------------------------------------------------------------------------
//Wrapping SEQUR-PQRND APIs

extern "C"
JNIEXPORT jlong
JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_pQrndInitJ(
        JNIEnv* env,
        jobject ) {
    QEEP_RET ret;
    pQrnd_Handle pHandle;
    ret = pQrndInit(&pHandle);
    if (ret != QEEP_OK)  pHandle = 0;
    return (unsigned long)(pHandle);
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_pQrndSeedLoadJ(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray seed,
        jint seed_len) {
    QEEP_RET ret;
    jbyte *jqk = (*env).GetByteArrayElements(seed, 0);
    ret = pQrndSeed((QP_Handle)(unsigned long)handle, (uint8_t*)jqk, seed_len);
    (*env).ReleaseByteArrayElements(seed, jqk, 0);
    return (int)ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_pQrndRndJ(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray randomVal,
        jint out_len) {
    QEEP_RET ret;
    jbyte *jout = (*env).GetByteArrayElements(randomVal, 0);
    ret = pQrndRnd((QP_Handle)(unsigned long)handle, (uint8_t*)jout, out_len);
    (*env).ReleaseByteArrayElements(randomVal, jout, 0);
    return (int)ret;
}

extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_sequrKotlinWrapper_pQrndCloseJ(
        JNIEnv* env,
        jobject /* this */,
        jlong handle) {
    QEEP_RET ret;
    ret = pQrndClose((pQrnd_Handle)(unsigned long)handle);
    return (int)ret;
}

//-------------------------------------------------------------------------------------------
//  Wrapping MASQ KEM APIs
// MASQ_KEM JNI functions

// MASQ KEM Qeep Init
extern "C"
JNIEXPORT jlong
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemQeepInit(
        JNIEnv* env,
        jobject obj,
        jint algo) {
    (void)env;
    (void)obj;

    MASQ_KEM_handle* handle_ptr;
    switch (algo) {
        case 1: handle_ptr = MASQ_KEM_qeep_init(KEM_LEVEL1); break;
        case 3: handle_ptr = MASQ_KEM_qeep_init(KEM_LEVEL3); break;
        case 5: handle_ptr = MASQ_KEM_qeep_init(KEM_LEVEL5); break;
        default: handle_ptr = nullptr;
    }
    return (jlong)handle_ptr;
}

// MASQ KEM used key format
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemUseKeyFormat(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    MASQ_KEM_API ret = MASQ_KEM_use_key_format((MASQ_KEM_handle*)handle);
    return (jint)ret;
}

// MASQ KEM Seed
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemSeed(
        JNIEnv* env,
        jobject obj,
        jlong handle,
        jbyteArray seed,
        jint seedLen) {
    (void)env;
    (void)obj;
    jbyte *jseed = (*env).GetByteArrayElements(seed, 0);
    if (!jseed) return JNI_ERR;

    MASQ_KEM_API ret = MASQ_KEM_seed((MASQ_KEM_handle*)handle, (uint8_t*)jseed, seedLen);
    (*env).ReleaseByteArrayElements(seed, jseed, 0);
    return (jint)ret;
}

// MASQ KEM Public Key Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemPublicKeyLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_KEM_public_key_length((MASQ_KEM_handle*)handle);
}

// MASQ KEM Secret Key Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemSecretKeyLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_KEM_secret_key_length((MASQ_KEM_handle*)handle);
}

// MASQ KEM Ciphertext Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemCiphertextLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_KEM_ciphertext_length((MASQ_KEM_handle*)handle);
}

// MASQ KEM Shared Secret Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemSharedSecretLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_KEM_shared_secret_length((MASQ_KEM_handle*)handle);
}

// MASQ KEM Key Pair Generation
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemKeypair(
        JNIEnv* env,
        jobject obj,
        jlong handle,
        jbyteArray publicKey,
        jbyteArray secretKey) {
    (void)env;
    (void)obj;

    jbyte* jpublicKey = (*env).GetByteArrayElements(publicKey, 0);
    jbyte* jsecretKey = (*env).GetByteArrayElements(secretKey, 0);
    if (!jpublicKey || !jsecretKey) return JNI_ERR;

    MASQ_KEM_API ret = MASQ_KEM_keypair((MASQ_KEM_handle*)handle,
                                        (uint8_t*)jpublicKey,
                                        (uint8_t*)jsecretKey);

    (*env).ReleaseByteArrayElements(publicKey, jpublicKey, 0);
    (*env).ReleaseByteArrayElements(secretKey, jsecretKey, 0);

    return (jint)ret;
}

// MASQ KEM Encapsulation
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemEncaps(
        JNIEnv* env,
        jobject obj,
        jlong handle,
        jbyteArray publicKey,
        jbyteArray sharedSecret,
        jbyteArray ciphertext) {
    (void)env;
    (void)obj;
    jbyte *jciphertext = (*env).GetByteArrayElements(ciphertext, 0);
    jbyte *jsharedSecret = (*env).GetByteArrayElements(sharedSecret, 0);
    jbyte *jpublicKey = (*env).GetByteArrayElements(publicKey, 0);
    if (!jciphertext) return JNI_ERR;
    if (!jpublicKey) return JNI_ERR;
    if (!jsharedSecret) return JNI_ERR;

    MASQ_KEM_API ret = MASQ_KEM_encaps((MASQ_KEM_handle*)handle,
                                       (uint8_t*)jpublicKey,
                                       (uint8_t*)jsharedSecret,
                                       (uint8_t*)jciphertext);

    (*env).ReleaseByteArrayElements(ciphertext, jciphertext, 0);
    (*env).ReleaseByteArrayElements(sharedSecret, jsharedSecret, 0);
    (*env).ReleaseByteArrayElements(publicKey, jpublicKey, 0);

    return (jint)ret;
}

// MASQ KEM Decapsulation
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemDecaps(
        JNIEnv* env,
        jobject obj,
        jlong handle,
        jbyteArray secretKey,
        jbyteArray ciphertext,
        jbyteArray sharedSecret) {
    (void)env;
    (void)obj;
    jbyte *jsharedSecret = (*env).GetByteArrayElements(sharedSecret, 0);
    jbyte *jciphertext = (*env).GetByteArrayElements(ciphertext, 0);
    jbyte *jsecretKey = (*env).GetByteArrayElements(secretKey, 0);
    if (!jciphertext) return JNI_ERR;
    if (!jsecretKey) return JNI_ERR;
    if (!jsharedSecret) return JNI_ERR;

    MASQ_KEM_API ret = MASQ_KEM_decaps((MASQ_KEM_handle*)handle,
                                       (uint8_t*)jsecretKey,
                                       (uint8_t*)jciphertext,
                                       (uint8_t*)jsharedSecret);

    (*env).ReleaseByteArrayElements(sharedSecret, jsharedSecret, 0);
    (*env).ReleaseByteArrayElements(ciphertext, jciphertext, 0);
    (*env).ReleaseByteArrayElements(secretKey, jsecretKey, 0);

    return (jint)ret;
}

// MASQ KEM Free
extern "C"
JNIEXPORT void
JNICALL Java_com_example_demoqispaceapp_masqKemKotlinWrapper_masqKemQeepFree(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    MASQ_KEM_qeep_free((MASQ_KEM_handle*)handle);
}

//-------------------------------------------------------------------------------------------
//  Wrapping MASQ DS APIs
// MASQ DS JNI functions

// MASQ DS QEEP init
extern "C"
JNIEXPORT jlong
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsQeepInit(
        JNIEnv* env,
        jobject /* this */,
        jint algo) {

    MASQ_DS_handle* handle_ptr;
    switch (algo) {
        case 1: handle_ptr = MASQ_DS_qeep_init(DS_LEVEL1); break;
        case 3: handle_ptr = MASQ_DS_qeep_init(DS_LEVEL3); break;
        case 5: handle_ptr = MASQ_DS_qeep_init(DS_LEVEL5); break;
        default: handle_ptr = nullptr;
    }
    return (jlong)handle_ptr;
}

// MASQ DS Seed
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsSeed(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray seed,
        jint seedLen) {
    jbyte *jseed = (*env).GetByteArrayElements(seed, 0);
    if (!jseed) return JNI_ERR;

    MASQ_DS_API ret = MASQ_DS_seed((MASQ_DS_handle*)handle, (uint8_t*)jseed, seedLen);
    (*env).ReleaseByteArrayElements(seed, jseed, 0);
    return (jint)ret;
}

// MASQ DS Public Key Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsPublicKeyLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_DS_public_key_length((MASQ_DS_handle*)handle);
}

// MASQ DS Private Key Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsPrivateKeyLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return MASQ_DS_private_key_length((MASQ_DS_handle*)handle);
}

// MASQ DS Signature Length
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsSignatureLength(
        JNIEnv* env,
        jobject obj,
        jlong handle) {
    (void)env;
    (void)obj;

    return (jint)MASQ_DS_signature_length((MASQ_DS_handle*)handle);
}

// MASQ DS Key Pair Generation
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsKeypair(
        JNIEnv* env,
        jobject obj,
        jlong handle,
        jbyteArray publicKey,
        jbyteArray privateKey) {
    (void)env;
    (void)obj;

    jbyte *jpublicKey = (*env).GetByteArrayElements(publicKey, 0);
    jbyte *jprivateKey = (*env).GetByteArrayElements(privateKey, 0);
    if (!jpublicKey || !jprivateKey) return JNI_ERR;

    MASQ_DS_API ret = MASQ_DS_keypair((MASQ_DS_handle*)handle,
                                      (uint8_t*)jpublicKey,
                                      (uint8_t*)jprivateKey);

    (*env).ReleaseByteArrayElements(publicKey, jpublicKey, 0);
    (*env).ReleaseByteArrayElements(privateKey, jprivateKey, 0);

    return (jint)ret;
}

// MASQ DS Sign
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsSign(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray privateKey,
        jbyteArray message,
        jint messageLen,
        jbyteArray signature,
        jint signatureLen) {
    jbyte *jprivateKey = (*env).GetByteArrayElements(privateKey, 0);
    jbyte *jmessage = (*env).GetByteArrayElements(message, 0);
    jbyte *jsignature = (*env).GetByteArrayElements(signature, 0);

    int32_t sig_len = signatureLen;

    MASQ_DS_API ret = MASQ_DS_sign((MASQ_DS_handle*)handle,
                                   (uint8_t*)jprivateKey,
                                   (uint8_t*)jmessage,
                                   messageLen,
                                   (uint8_t*)jsignature,
                                   &sig_len,
                                   NULL,
                                   0);

    (*env).ReleaseByteArrayElements(privateKey, jprivateKey, 0);
    (*env).ReleaseByteArrayElements(message, jmessage, 0);
    (*env).ReleaseByteArrayElements(signature, jsignature, 0);

    return (jint)ret;
}

// MASQ DS Verify
extern "C"
JNIEXPORT jint
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsVerify(
        JNIEnv* env,
        jobject /* this */,
        jlong handle,
        jbyteArray publicKey,
        jbyteArray message,
        jint messageLen,
        jbyteArray signature,
        jint signatureLen) {

    jbyte *jpublicKey = (*env).GetByteArrayElements(publicKey, 0);
    jbyte *jmessage = (*env).GetByteArrayElements(message, 0);
    jbyte *jsignature = (*env).GetByteArrayElements(signature, 0);

    MASQ_DS_API ret = MASQ_DS_verify((MASQ_DS_handle*)handle,
                                     (uint8_t*)jpublicKey,
                                     (uint8_t*)jmessage,
                                     messageLen,
                                     (uint8_t*)jsignature,
                                     signatureLen,
                                     NULL,
                                     0);

    (*env).ReleaseByteArrayElements(publicKey, jpublicKey, 0);
    (*env).ReleaseByteArrayElements(message, jmessage, 0);
    (*env).ReleaseByteArrayElements(signature, jsignature, 0);

    return (jint)ret;
}

// MASQ DS Free
extern "C"
JNIEXPORT void
JNICALL Java_com_example_demoqispaceapp_masqDsKotlinWrapper_masqDsQeepFree(
        JNIEnv* env,
        jobject /* this */,
        jlong handle) {
    MASQ_DS_handle ds_handle = (MASQ_DS_handle)(unsigned long)handle;
    MASQ_DS_handle *ds_ptr = &ds_handle;

    MASQ_DS_qeep_free(ds_ptr);
}