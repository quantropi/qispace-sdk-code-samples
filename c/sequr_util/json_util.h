#ifndef _JSON_UTIL_H_
#define _JSON_UTIL_H_

#ifdef  __cplusplus
extern "C" {
#endif

int json_get_values(char* json_str, char** url, char** token, char** subkey, char** iv, char** id, char** qk);

#ifdef  __cplusplus
}
#endif

#endif