
#ifndef _QISPACE_DEMO_RESTAPIUTIL_
#define _QISPACE_DEMO_RESTAPIUTIL_


std::string QiSpaceAPI_call(char const *method, const std::string& url, const std::string& token, const std::string& body);
int QiSpaceAPI_subKey(std::string& subkey);
int QiSpaceAPI_QK(std::string& encodedQK, std::string& iv, int key_length);
int QiSpaceAPI_QE(int len, std::string& encodedQE, std::string& iv);


#endif  /*_QISPACE_DEMO_RESTAPIUTIL_*/