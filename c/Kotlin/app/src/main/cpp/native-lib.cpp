#include <jni.h>
#include <string>
// Path is based on current directory
#include "../../../../QiSpace_SDK_Lib/prebuilt/qispace_pqrnd.h"
#include "../../../../QiSpace_SDK_Lib/prebuilt/qispace_qeep.h"
#include "../../../../QiSpace_SDK_Lib/prebuilt/qispace_sequr.h"
#include "../../../../QiSpace_SDK_Lib/prebuilt/error.h"

// This file calls the QEEP SDK APIs
// It maps each function emntioned in sequr.h and qeep.h to a JNI function,
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
//DONE
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