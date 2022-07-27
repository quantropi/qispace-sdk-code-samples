
#ifndef _QISPACE_DEMO_RESTAPIUTIL_
#define _QISPACE_DEMO_RESTAPIUTIL_

#ifdef  __cplusplus
extern "C" {
#endif

int QiSpaceAPI_subKey(char* subkey);
int QiSpaceAPI_QK(char* encodedQK, char *iv, int key_length);
int QiSpaceAPI_QE(int len, char* encodedQE, char *iv);

#ifdef  __cplusplus
}
#endif

#endif  /*_QISPACE_DEMO_RESTAPIUTIL_*/