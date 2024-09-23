#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef  __cplusplus
extern "C" {
#endif
typedef unsigned char BYTE;

int base64_encode(BYTE const* buf, unsigned int bufLen, char *str_base64);
int base64_decode(char const* str_base64, int str_len, BYTE *buf);

#ifdef  __cplusplus
}
#endif

#endif



