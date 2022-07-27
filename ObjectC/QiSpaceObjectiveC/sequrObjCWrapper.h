#ifndef sequrObjWrapper_h
#define sequrObjCWrapper_h

#import "../QiSpace_SDK_Lib/qispace_pqrnd.h"


int SEQUR_decode(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *encoded_msg, int32_t encoded_msg_len, uint8_t * decoded_msg);

int pQrnd_setup(uint8_t *randSeed, int randSeed_len, pQrnd_Handle *randHandle);
int pQrnd_generate(pQrnd_Handle randHandle, uint8_t *rand, int rand_len);
int pQrnd_close(pQrnd_Handle randHandle);

#endif /* qscWrapper_h */
