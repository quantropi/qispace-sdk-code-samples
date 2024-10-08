#ifndef _BASE64_H_
#define _BASE64_H_

#include <string>

typedef unsigned char BYTE;

int base64_decode(const std::string& encoded_string, int str_len, BYTE *out_buf);


#endif