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
#include <sstream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cjson/cJSON.h> 
#include <curl/curl.h>
#include "sequr_util.h"
#include "help_util.h"
#include "json_util.h"

using namespace std;

#define QK_TAG_LENGTH  18
#define QK_HASH_LENGTH  4

// Function to initialize SequrHandle with qispace_meta
SequrHandle* sequr_util_init(const std::string q_meta) {

    long response_code = 0;
    std::string res;
    std::string full_url;
    std::string subkey;
    QP_Handle qp_handle; // Create QEEP handle
    int ret = 0;

    if(q_meta.empty()) { return nullptr; }
   
    // Initialize SequrHandle
    SequrHandle *handle = new SequrHandle();
    if (!handle) { return nullptr; }

    handle->q_meta = new qispace_meta();
    if (!handle->q_meta) {
        sequr_free(handle);
        return nullptr;
    }

    ret = json_get_values(q_meta, "url", handle->q_meta->url);
    if(ret != 0){
        sequr_free(handle);
        return nullptr;
    }

    ret = json_get_values(q_meta, "token", handle->q_meta->token);
    if(ret != 0){
        sequr_free(handle);
        return nullptr;
    }
    
    // Perform HTTPS POST request to retrieve the subkey
    full_url = handle->q_meta->url + "/sub_key";
    res = QiSpaceAPI_call("POST", full_url, handle->q_meta->token, "", &response_code);
    if (response_code != 200) {
        sequr_free(handle);
        return nullptr;
    }

    // convert subkey from hex string to byte array
    ret = json_get_values(res, "sub_key", subkey);
    if(ret != 0){
        sequr_free(handle);
        return nullptr;
    }

    std::vector<uint8_t> sub_key(subkey.size() / 2);
    for (size_t i = 0; i < sub_key.size(); i++) {
        sscanf(subkey.c_str() + 2*i, "%02hhx", &sub_key[i]);
    }

    // Initialize QP_Handle
    if (QP_init(&qp_handle) != QEEP_OK){ 
        sequr_free(handle);
        return nullptr;
    }

    // Load subkey
    if (QP_qeep_key_load(qp_handle, sub_key.data(), sub_key.size()) != QEEP_OK) { 
        sequr_free(handle);
        return nullptr;
    }

    // Assign the initialized qp_handle to the SequrHandle
    handle->qp_handle = qp_handle;

    return handle;
}


/*
Function to generate key
  - key_size: input parameter in bytes
  - key_type: 0 for AES key, 1 for QEEP KEY
  - key:   key buffer should be allocated before calling this function
           key_buffer_size >= key_size + 22 if key_type is QEEP. 
*/
int sequr_util_key_gen(SequrHandle* sequr_handle, int32_t key_size, std::string& key_id, uint8_t *key, int key_type) {
    char const *label = "sequr";
    int ret = 0;
    long response_code = 0;
    std::string full_url;
    std::string res;
    std::string iv_b64;
    std::string id;
    std::string QK_b64;
    QP_Handle qp_handle;
    uint8_t iv[17]; 
    int32_t iv_len; 
    int encoded_QK_len=0;
    uint8_t* encoded_QK;
    uint8_t* buf;

    if(sequr_handle == nullptr || key_size <= 0 || key == nullptr) { return (-1);}
    if(sequr_handle->q_meta == nullptr || sequr_handle->q_meta->url.empty() || sequr_handle->q_meta->token.empty()) { return (-1);}

    // Perform HTTPS POST request to generate QK
    std::ostringstream post_body_stream;
    post_body_stream << "{ \"key_length\" : " << key_size << ", \"label\": \"" << label << "\" }";

    std::string body_data = post_body_stream.str();
    
    full_url = std::string(sequr_handle->q_meta->url) + "/qk";
    
    res = QiSpaceAPI_call("POST", full_url, sequr_handle->q_meta->token, body_data, &response_code);
    if (response_code != 200) {
        return -1;
    }

    // get iv, id and payload from response
    ret = json_get_values(res, "iv", iv_b64);
    if (ret != 0) { return (-1);}
    ret = json_get_values(res, "id", id);
    if (ret != 0) { return (-1);}
    ret = json_get_values(res, "payload", QK_b64); //this needs to be payload, not qk
    if (ret != 0) { return (-1);}
    
    // get key_id
    key_id = id;

    // Create QEEP handle
    qp_handle = sequr_handle->qp_handle;
    if (qp_handle == nullptr) { return (-1);}

    // decode iv from base64 format to byte array
    iv_len = base64_decode(iv_b64, iv_b64.length(), iv);
    // load iv
    ret = QP_iv_set(qp_handle, iv, iv_len);
    if (ret != QEEP_OK) { return (-1); }

    encoded_QK_len = key_size + QK_TAG_LENGTH + QK_HASH_LENGTH;
    encoded_QK = new uint8_t[encoded_QK_len];

    // decode QK from base64 format to byte array 
    encoded_QK_len = base64_decode(QK_b64, QK_b64.size(), encoded_QK);

    // decode QK to plaintext byte array
    buf = new uint8_t[encoded_QK_len];
    ret = QP_decrypt(qp_handle, encoded_QK, encoded_QK_len, buf);  
    if (ret != QEEP_OK) { 
        delete[] encoded_QK;
        delete[] buf;
        return (-1);
        }

    // Copy the relevant part of buf (skipping the first QK_TAG_LENGTH and ignoring the last QK_HASH_LENGTH)
    if (key_type ==1 ) {
        memcpy(key, buf, encoded_QK_len);
        ret = encoded_QK_len;
    }
     else  {
        memcpy(key, buf + QK_TAG_LENGTH, key_size);
        ret = key_size;
     }

    // free data
    delete[] encoded_QK;
    delete[] buf;
    
    return (ret);
}

/*
 Function to get key by key_id
  - key_type: 0 for AES KEY, 1 for QEEP KEY
  - key:   key buffer should be allocated before calling this function
           key_buffer_size >= key_size + 22 if key_type is QEEP. 
           */
int sequr_util_query_key(SequrHandle* sequr_handle, std::string& key_id, uint8_t *key, int key_type) {
    
    long response_code = 0;
    int ret = 0;
    std::string full_url;
    std::string res;
    std::string iv_b64;
    std::string QK_b64;
    uint8_t iv[17];
    int32_t iv_len;
    QP_Handle qp_handle;
    int b64_QK_len = 0;
    int encoded_QK_len = 0;
    int key_size;
    uint8_t *encoded_QK;
    uint8_t *buf;

    if(sequr_handle == nullptr || key == nullptr) { return (-1); }
    if(sequr_handle->q_meta == nullptr || sequr_handle->q_meta->url.empty() || sequr_handle->q_meta->token.empty()) { return (-1);}

    // Perform HTTPS GET request to get QK
    full_url = sequr_handle->q_meta->url + "/qk/" + key_id.c_str();

    res = QiSpaceAPI_call("GET", full_url, sequr_handle->q_meta->token, "", &response_code);
    if (response_code != 200) {
        return -1;
    }

    ret = json_get_values(res, "iv", iv_b64);
    if (ret != 0) { return (-1);}
    ret = json_get_values(res, "payload", QK_b64); //this needs to be payload, not qk
    if (ret != 0) { return (-1);}
   
    // Create QEEP handle
    qp_handle = sequr_handle->qp_handle;
    if (qp_handle == nullptr) { return (-1);}
    
    // decode iv from base64 format to byte array
    iv_len = base64_decode(iv_b64, iv_b64.length(), iv);
    // load iv
    ret = QP_iv_set(qp_handle, iv, iv_len);
    if (ret != QEEP_OK) { return (-1);}

    b64_QK_len = QK_b64.length();
    encoded_QK = new uint8_t[b64_QK_len];
    
    // decode QK from base64 format to byte array 
    encoded_QK_len = base64_decode(QK_b64, QK_b64.length(), encoded_QK);
    // decode QK to plaintext byte array
    buf =  new uint8_t[encoded_QK_len];
    ret = QP_decrypt(qp_handle, encoded_QK, encoded_QK_len, buf);    
    if (ret != QEEP_OK) { 
        delete[] encoded_QK;
        delete[] buf;
        return -1;
    }
    
    // Copy the relevant part of buf (skipping the first QK_TAG_LENGTH and ignoring the last QK_HASH_LENGTH)
    if (key_type ==1 ) {
        key_size = encoded_QK_len;
        memcpy(key, buf, key_size);
    }
     else  {
        key_size = encoded_QK_len - QK_TAG_LENGTH - QK_HASH_LENGTH;
        memcpy(key, buf + QK_TAG_LENGTH, key_size);
     }
    // free data
    delete[] encoded_QK;
    delete[] buf;

    return key_size;
}



// Free SequrHandle
void sequr_free(SequrHandle* sequr_handle) {
    if (!sequr_handle) {
        return;
    }
    
    if (sequr_handle->q_meta) {
        sequr_handle->q_meta->url.clear();
        sequr_handle->q_meta->token.clear();
        sequr_handle->q_meta = nullptr;
    }

    if (sequr_handle->qp_handle) {
        QP_close(sequr_handle->qp_handle);  // Clean up the QP handle
        sequr_handle->qp_handle = nullptr;
    }

   // Finally free the sequr_handle
    sequr_handle = nullptr;
}