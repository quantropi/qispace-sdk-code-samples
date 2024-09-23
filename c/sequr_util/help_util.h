#ifndef _HELP_UTIL_H_
#define _HELP_UTIL_H_

#ifdef  __cplusplus
extern "C" {
#endif
typedef unsigned char BYTE;

char* QiSpaceAPI_call(const char *method, char const *url, char const *token, char const *body, long *response_code);

int base64_decode(const char* encoded_string, int str_len, BYTE *out_buf);

#ifdef  __cplusplus
}
#endif

#endif