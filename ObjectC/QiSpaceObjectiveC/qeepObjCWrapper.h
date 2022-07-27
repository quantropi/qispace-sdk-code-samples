#ifndef qeepObjWrapper_h
#define qeepObjCWrapper_h

int QEEP_encrypt(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *msg, int32_t msg_len, uint8_t * encoded_msg);
int QEEP_decrypt(uint8_t *qk, int32_t qk_len, uint8_t *iv, uint8_t *encoded_msg, int32_t encoded_msg_len, uint8_t * decoded_msg);

#endif

