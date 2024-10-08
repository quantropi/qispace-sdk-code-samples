
#ifdef QISPACE_API_ON

/*
The util depends on nlohmann json https://github.com/nlohmann/json.git
and curl
*/
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <nlohmann/json.hpp> 
#include <curl/curl.h>
#include "RestApiUtil.h"
#include "qispace_info.h"


using namespace std;
using json = nlohmann::json;

static size_t writefunc(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t total_size = size * nmemb;

    // Convert contents to a string
    if (response) {
        response->append(static_cast<char*>(contents), total_size);
    }

    return total_size;
}


/* Blueprint method for HTTP requests involving QiSpace API */
std::string QiSpaceAPI_call(char const *method, const std::string& url, const std::string& token, const std::string& body){
    CURL *curl; 
    CURLcode res;
    std::string response;

    static bool curl_initialized = false;
    if (!curl_initialized) { // Initialize libcurl globally
        curl_global_init(CURL_GLOBAL_ALL);
        curl_initialized = true;
    }
    
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
        return response;
    }

    /* configuring HTTP request with necessary specifications */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    if (!token.empty()) {
        std::string auth_header = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, auth_header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);   //remove this to disable verbose output

    if (!body.empty()) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    }

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){ /* Check for errors */
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        response.clear();
    } 
            
    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

/* Call QiSpace API to establish a shared sub_key */
int QiSpaceAPI_subKey( std::string& subkey ) {
  /* Setup URL and access token */
    std::string full_url = QISPACE_ENTERPRISE_BASE_URL;
    full_url = full_url + "/api/v1/sub_key";
    
    std::string res = QiSpaceAPI_call("POST", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN, "");
    
    /* Extract the subkey string from json response */
    if (res.size() > 0 ) {
      json ResJson = json::parse(res);

      std::string sk = ResJson["sub_key"];
      subkey = sk;

      return 1;
    }
    return 0;
}

/* Call QiSpace API to obtain Qeep Key (QK) */
int QiSpaceAPI_QK( std::string& encoded_QK, std::string& iv, int key_length) {
    
    /* Setup URL and access token */
    char const *label = "test";
    
    std::ostringstream post_body_stream;
    post_body_stream << "{ \"key_length\" : " << key_length << ", \"label\": \"" << label << "\" }";

    std::string body_data = post_body_stream.str();
    
    std::string full_url = QISPACE_ENTERPRISE_BASE_URL;
    full_url = full_url + "/api/v1/qk";

    std::string res = QiSpaceAPI_call("POST", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN, body_data);
    /* Extract QK and IV string from json response */
    if (res.size() > 0 ) {
      json ResJson = json::parse(res);

      std::string qk = ResJson["payload"];
      encoded_QK = qk;

      std::string ivc = ResJson["iv"];
      iv = ivc;

      return 1;
    }
    return 0;
}

/* Call QiSpace API to get Quantum Entropy (QE) */
int QiSpaceAPI_QE( int len, std::string& encoded_QE, std::string& iv) {
  /* Setup URL and access_token */
  
  std::string full_url = QISPACE_ENTERPRISE_BASE_URL;
  full_url = full_url + "/api/v1/qe/";

  char qe_len[10];
  sprintf(qe_len, "%d", len);
  full_url = full_url + qe_len;
  
  std::string res  = QiSpaceAPI_call("GET", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN, "");
  
  /* Extract QE and its IV string from json response */
  if (res.size() > 0 ) {
    json ResJson = json::parse(res);

    std::string qe = ResJson["payload"];
    encoded_QE = qe;

    std::string ivc = ResJson["iv"];
    iv = ivc;

    return 1;
  }
  return 0;
}


#endif  /*QISPACE_API_ON*/
