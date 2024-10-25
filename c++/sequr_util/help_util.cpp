/******************************************************************************
 *
 * Quantropi CONFIDENTIAL
 * ----------------------
 *
 * This file is proprietary code and subject to the terms and conditions
 * defined in the file 'LICENSE', which is part of this source code package.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Quantropi Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Quantropi Incorporated and its suppliers and
 * may be covered by Canadian, U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Quantropi Incorporated.
 *
 *****************************************************************************/

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <curl/curl.h>
#include "help_util.h"


using namespace std;

std::string read_file_to_string(const std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    
    if (!file) {
        return "";  // Return an empty string if the file cannot be opened
    }

    // Read the entire file into a string
    std::string file_content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    return file_content;  // Return the file content as a string
}

// Base64 character to value table
static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

// Check if a character is a valid Base64 character
bool is_base64(BYTE c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

// Base64 decode function
int base64_decode(const std::string& encoded_string, int str_len, BYTE *out_buf) {
    int in_len = str_len;
    int i = 0;
    int j = 0;
    int in_ = 0;
    BYTE char_array_4[4], char_array_3[3];
    int ret_len = 0;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) {
                out_buf[ret_len++] = char_array_3[i];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++) {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++) {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) {
            out_buf[ret_len++] = char_array_3[j];
        }
    }

    return ret_len;
}

// Callback function to handle data received from the server
static size_t writefunc(void *contents, size_t size, size_t nmemb, std::string *response) {
    size_t total_size = size * nmemb;

    // Convert contents to a string
    if (response) {
        response->append(static_cast<char*>(contents), total_size);
    }

    return total_size;
}


// Function to perform an HTTPS GET or POST request
std::string QiSpaceAPI_call(const char* method, const std::string& url, const std::string& token, const std::string& body, long *response_code) {
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
        std::cerr << "Failed to initialize CURL" << std::endl;
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
        
    if (response_code) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
    }
    
    /* always cleanup */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}
