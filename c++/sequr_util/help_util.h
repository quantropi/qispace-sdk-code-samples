#ifndef _HELP_UTIL_H_
#define _HELP_UTIL_H_


typedef unsigned char BYTE;

std::string read_file_to_string(const std::string &path);

std::string QiSpaceAPI_call(const char* method, const std::string& url, const std::string& token, const std::string& body, long *response_code);

int base64_decode(const std::string& encoded_string, int str_len, BYTE* out_buf);


#endif