
#ifdef QISPACE_API_ON

/*
The util depends on nlohmann json https://github.com/nlohmann/json.git
and curl
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <nlohmann/json.hpp> 
#include <curl/curl.h>
#include <string>
#include "RestApiUtil.h"
#include "qispace_info.h"


using namespace std;
using json = nlohmann::json;


struct _strings {
  char *ptr;
  size_t len;
};

void init_string(struct _strings *s) {
  s->len = 0;
  s->ptr = static_cast<char*>(malloc(s->len+1));
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct _strings *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = static_cast<char*>(realloc(s->ptr, new_len+1));
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}


/* Blueprint function for HTTP requests involving QiSpace API */
std::string QiSpaceAPI_call(char const *method, char const *url, char const *token, char const *body){
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    char *full_token = (char *)calloc(50 + strlen(token), sizeof(char));
    strcat(full_token, "Authorization: Bearer ");
    strcat(full_token, token);

    /* configuring HTTP request with necessary specifications */
    if(curl) {
        struct _strings s;
        init_string(&s);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
        struct curl_slist *headers = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, url);

        headers = curl_slist_append(headers, full_token);
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charset: utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L);   //remove this to disable verbose output
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        if(body != NULL){
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
        }

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);

        /* Check for errors */
        if(res != CURLE_OK){
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
          curl_easy_strerror(res));
          return NULL;
        } else {
            // return response in json
            // json ResJson = json::parse(s.ptr);
            // *data = &ResJson;
          return s.ptr;
            //return 1;
        }

    }
    fprintf(stderr, "curl_easy_init() failed:\n");
    return NULL;
}

/* Call QiSpace API to establish a shared sub_key */
int QiSpaceAPI_subKey( char* subkey) {
  /* Setup URL and access token */
    char* body_data = NULL;
    char *full_url = (char *)calloc(strlen(QISPACE_ENTERPRISE_BASE_URL) + 50, sizeof(char));
    strcat(full_url, QISPACE_ENTERPRISE_BASE_URL);
    strcat(full_url, "/api/v1/sub_key");
    std::string res = QiSpaceAPI_call("POST", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN,  body_data);
    free(full_url);

    /* Extract the subkey string from json response */
    if (res.size() > 0 ) {
      json ResJson = json::parse(res);

      std::string sk = ResJson["sub_key"];
      strcpy(subkey, sk.c_str());

      return 1;
    }
    return 0;
}

/* Call QiSpace API to obtain Qeep Key (QK) */
int QiSpaceAPI_QK( char* encoded_QK, char *iv, int key_length) {
    // int key_length = 64;
    /* Setup URL and access token */
    char const *label = "test";

    char body_data[1000];
    int ret = sprintf(body_data, "{ \"key_length\" : %d, \"label\": \"%s\" }", key_length, label);

    char *full_url = (char *)calloc(strlen(QISPACE_ENTERPRISE_BASE_URL) + 50, sizeof(char));
    strcat(full_url, QISPACE_ENTERPRISE_BASE_URL);
    strcat(full_url, "/api/v1/qk");

    std::string res = QiSpaceAPI_call("POST", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN, body_data);
    free(full_url);
    /* Extract QK and IV string from json response */
    if (res.size() > 0 ) {
      json ResJson = json::parse(res);

      std::string qk = ResJson["payload"];
      strcpy(encoded_QK, qk.c_str());

      std::string ivc = ResJson["iv"];
      strcpy(iv, ivc.c_str());

      return 1;
    }
    return 0;
}

/* Call QiSpace API to get Quantum Entropy (QE) */
int QiSpaceAPI_QE( int len, char* encoded_QE, char *iv) {
  /* Setup URL and access_token */
  char *body_data = NULL;

  char *full_url = (char *)calloc(strlen(QISPACE_ENTERPRISE_BASE_URL) + 50, sizeof(char));
  strcat(full_url, QISPACE_ENTERPRISE_BASE_URL);
  strcat(full_url, "/api/v1/qe/");
  char qe_len[10];
  sprintf(qe_len, "%d", len);
  strcat(full_url, qe_len);

  std::string res  = QiSpaceAPI_call("GET", full_url, QISPACE_ENTERPRISE_ACCESS_TOKEN, body_data);

  free(full_url);
  /* Extract QE and its IV string from json response */
  if (res.size() > 0 ) {
    json ResJson = json::parse(res);

    std::string qe = ResJson["payload"];
    strcpy(encoded_QE, qe.c_str());

    std::string ivc = ResJson["iv"];
    strcpy(iv, ivc.c_str());

    return 1;
  }
  return 0;
}

#endif  /*QISPACE_API_ON*/
