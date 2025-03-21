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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "sequr_util.h"
#include "help_util.h"
#include "json_util.h"
#include "qispace_qeep.h"


#define QK_TAG_LENGTH  18
#define QK_HASH_LENGTH  4

// Function to initialize sequr_handle with qispace_meta
sequr_handle* sequr_util_init(char* q_meta) {
    char* body_data = NULL;
    long response_code;
    char *res = NULL;
    char *subkey = NULL;
    uint8_t sub_key[1100];
    size_t subkey_len = 0;
    QP_Handle qp_handle; // Create QEEP handle
    int i = 0;
    char *full_url = NULL;

    if(!q_meta) { return NULL; }
   
    // Initialize sequr_handle
    sequr_handle *handle = (sequr_handle *)malloc(sizeof(sequr_handle));
    if (!handle) { return NULL; }

    handle->q_meta = (qispace_meta *)malloc(sizeof(qispace_meta));
    if (!handle->q_meta) {
        free(handle);
        return NULL;  // Error: memory allocation failed
    }

    handle->q_meta->url = NULL;
    handle->q_meta->token = NULL;
    int ret = json_get_values(q_meta, &(handle->q_meta->url), &(handle->q_meta->token), NULL, NULL, NULL, NULL);
    if (ret != 0 ) {
        sequr_free(handle);
        return NULL;  // Error: memory allocation failed
    }

    // Perform HTTPS POST request to retrieve the subkey
    full_url = (char *)calloc(strlen(handle->q_meta->url) + 50, sizeof(char));
    if (full_url == NULL) { 
        sequr_free(handle);
        return NULL;
    }
    strcat(full_url, handle->q_meta->url);
    strcat(full_url, "/sub_key");
    res = QiSpaceAPI_call("POST", full_url, handle->q_meta->token, body_data, &response_code);
    free(full_url);
    if (response_code != 200) {
        free(res);
        sequr_free(handle);
        return NULL;
    }

    // convert subkey from hex string to byte array
    ret = json_get_values(res, NULL, NULL, &subkey, NULL, NULL, NULL);
    free(res);
    if (ret != 0) {
        sequr_free(handle);
        return NULL;  // Error: sub_key not found
    }

    subkey_len = strlen(subkey)/2;
    for(i = 0; i< subkey_len; i++) 
        sscanf((char*)&subkey[2*i], "%02x", (unsigned int *)&sub_key[i]);


    // Initialize QP_Handle
    ret = QP_init(&qp_handle);
    if (ret != QEEP_OK) { 
        sequr_free(handle);
        return NULL;
    }

    // Load subkey
    ret = QP_qeep_key_load(qp_handle, sub_key, subkey_len);
    if (ret != QEEP_OK) { 
        sequr_free(handle);
        return NULL;
    }

    // Assign the initialized qp_handle to the sequr_handle
    handle->qp_handle = qp_handle;
    free(subkey);
    return handle;
}


/*
Function to generate key
  - key_size in bytes
  - key_type: 0 for AES key, 1 for QEEP KEY, 2 for QEEP PASS
  - key: key buffer, which should be allocated before calling this function
            key_buffer_size >= key_size + key_type_header_size if key_type is QEEP. 
*/
int sequr_util_key_gen(sequr_handle* sequr_handle, int32_t key_size, char* key_id, uint8_t *key, int key_type) {
    char const *label = "sequr";
    char body_data[1000];
    int ret;
    long response_code;
    char *full_url = NULL;
    char *res = NULL;
    char *iv_b64 = NULL;
    char *id = NULL;
    char *QK_b64 = NULL;
    QP_Handle qp_handle;
    uint8_t iv[50];         
    int32_t iv_len; 
    int encoded_QK_len=0;
    uint8_t *encoded_QK;
    uint8_t *buf;

    if(sequr_handle == NULL || key_size <= 0 || key_id == NULL || key == NULL) { return (-1);}
    if(sequr_handle->q_meta == NULL || sequr_handle->q_meta->url == NULL || sequr_handle->q_meta->token == NULL) { return (-1);}

    // Perform HTTPS POST request to generate QK
    ret = sprintf(body_data, "{ \"key_length\" : %d, \"label\": \"%s\" }", key_size, label);
    full_url = (char *)calloc(strlen(sequr_handle->q_meta->url) + 50, sizeof(char));
    if (full_url == NULL) { return (-1);}
    strcat(full_url, sequr_handle->q_meta->url);
    strcat(full_url, "/qk");

    res = QiSpaceAPI_call("POST", full_url, sequr_handle->q_meta->token, body_data, &response_code);
    free(full_url);
    if (response_code != 200) {
        free(res);
        return -1;
    }

    // get iv, id and pyload from response
    ret = json_get_values(res, NULL, NULL, NULL, &iv_b64, &id, &QK_b64);
    free(res);
    if (ret != 0) { return (-1);}
    
    // get key_id
    strcpy(key_id, id);

    // Create QEEP handle
    qp_handle = sequr_handle->qp_handle;
    if (qp_handle == NULL) { return (-1);}

    // decode iv from base64 format to byte array
    iv_len = base64_decode(iv_b64, strlen(iv_b64), iv);
    // load iv
    ret = QP_iv_set(qp_handle, iv, iv_len);
    if (ret != QEEP_OK) { return (-1); }

    encoded_QK_len = key_size + QK_TAG_LENGTH + QK_HASH_LENGTH;
    encoded_QK = (uint8_t*)malloc(encoded_QK_len);
    
    // decode QK from base64 format to byte array 
    encoded_QK_len = base64_decode(QK_b64, strlen(QK_b64), encoded_QK);

    // decode QK to plaintext byte array
    buf = (uint8_t*)malloc(encoded_QK_len);
    ret = QP_decrypt(qp_handle, encoded_QK, encoded_QK_len, buf);    
    if (ret != QEEP_OK) { 
        free(encoded_QK);
        free(buf);
        return (-1);
    }

    // Copy the relevant part of buf (skipping the first QK_TAG_LENGTH and ignoring the last QK_HASH_LENGTH)
    if (key_type == (int)QEEP_KEY) {
        //QEEP_KEY
        ret = encoded_QK_len;
        memcpy(key, buf, encoded_QK_len);
    } else if (key_type == (int)QEEP_PASS) {
        //QEEP_PASS
        key[0] = 2;
        key[1] = (encoded_QK_len/256) & 0xff;
        key[2] = (encoded_QK_len) & 0xff;
        ret = encoded_QK_len + 2;
        memcpy(&key[3], &buf[1], encoded_QK_len -1);
    }
    else {
        //AES_KEY
        memcpy(key, buf + QK_TAG_LENGTH, key_size);
        ret = key_size;
    }

    // free data
    free(encoded_QK);
    free(buf);
    free(iv_b64);
    free(id);
    free(QK_b64);

    return (ret);
}

/*
 Function to get key by key_id
  - key_type: 0 for AES KEY, 1 for QEEP KEY, 2 for QEEP PASS
  - key:   key buffer should be allocated before calling this function
           key_buffer_size >= key_size + key_type_header_size if key_type is QEEP. 
*/
int sequr_util_query_key(sequr_handle* sequr_handle, char* key_id, uint8_t *key, int key_type) {
    char* body_data = NULL;
    long response_code;
    char *full_url = NULL;
    int ret = 0;
    char *res = NULL;
    char *iv_b64 = NULL;
    char *QK_b64 = NULL;
    uint8_t iv[50];         
    int32_t iv_len;
    QP_Handle qp_handle;
    int b64_QK_len;
    int encoded_QK_len;
    int key_size;
    uint8_t *encoded_QK;
    uint8_t *buf;

    if(sequr_handle == NULL || key_id == NULL || key == NULL) { return (-1); }
    if(sequr_handle->q_meta == NULL || sequr_handle->q_meta->url == NULL || sequr_handle->q_meta->token == NULL) { return (-1); }

    // Perform HTTPS GET request to get QK
    full_url = (char *)calloc(strlen(sequr_handle->q_meta->url) + 80, sizeof(char));
    if (full_url == NULL) { return (-1);}
    strcat(full_url, sequr_handle->q_meta->url);
    strcat(full_url, "/qk/");
    strcat(full_url, key_id);

    res = QiSpaceAPI_call("GET", full_url, sequr_handle->q_meta->token, body_data, &response_code);
    free(full_url);
    if (response_code != 200) {
        free(res);
        return -1;
    }

    ret = json_get_values(res, NULL, NULL, NULL, &iv_b64, NULL, &QK_b64);
    free(res);
    if (ret != 0) { return (-1);}
    
    // Create QEEP handle
    qp_handle = sequr_handle->qp_handle;
    if (qp_handle == NULL) { return (-1);}
    
    // decode iv from base64 format to byte array
    iv_len = base64_decode(iv_b64, strlen(iv_b64), iv);
    // load iv
    ret = QP_iv_set(qp_handle, iv, iv_len);
    if (ret != QEEP_OK) { return (-1);}

    b64_QK_len = strlen(QK_b64);
    encoded_QK = (uint8_t*)malloc(b64_QK_len);
    
    // decode QK from base64 format to byte array 
    encoded_QK_len = base64_decode(QK_b64, strlen(QK_b64), encoded_QK);
    // decode QK to plaintext byte array
    buf = (uint8_t*)malloc(encoded_QK_len);
    ret = QP_decrypt(qp_handle, encoded_QK, encoded_QK_len, buf);    
    if (ret != QEEP_OK) { 
        free(encoded_QK);
        free(buf);
        return (-1);
    }

    // Copy the relevant part of buf (skipping the first QK_TAG_LENGTH and ignoring the last QK_HASH_LENGTH)

    if (key_type == (int)QEEP_KEY) {
        //QEEP_KEY
        key_size = encoded_QK_len;
        memcpy(key, buf, key_size);
    } else if (key_type == (int)QEEP_PASS) {
        //QEEP_PASS
        key[0] = 2;
        key[1] = (encoded_QK_len/256) & 0xff;
        key[2] = (encoded_QK_len) & 0xff;
        key_size = encoded_QK_len + 2;
        memcpy(&key[3], &buf[1], encoded_QK_len -1);
    }
    else {
        //AES_KEY
        key_size = encoded_QK_len - QK_TAG_LENGTH - QK_HASH_LENGTH;
        memcpy(key, buf + QK_TAG_LENGTH, key_size);
    }

    // free data
    free(encoded_QK);
    free(buf);
    free(iv_b64);
    free(QK_b64);

    return key_size;
}

// Free handle
void sequr_free(sequr_handle* sequr_handle) {
    if(sequr_handle == NULL) { return; }
    
    if (sequr_handle->q_meta != NULL) {
        if (sequr_handle->q_meta->url != NULL) {
            free(sequr_handle->q_meta->url);
            sequr_handle->q_meta->url = NULL;
        }
        if (sequr_handle->q_meta->token != NULL) {
            free(sequr_handle->q_meta->token);
            sequr_handle->q_meta->token = NULL;
        }
        free(sequr_handle->q_meta);
        sequr_handle->q_meta = NULL;
    }

    if (sequr_handle->qp_handle != NULL) {
        QP_close(sequr_handle->qp_handle);  // Clean up the QP handle
        sequr_handle->qp_handle = NULL;
    }

    free(sequr_handle);  // Finally free the sequr_handle
    sequr_handle = NULL;
}

/*
 Function to query Quantum Entropy (QE)
  - QE: pointer to the QE buffer, which should be allocated before calling this function
  - len: required length of QE
*/
int sequr_util_get_qe(sequr_handle* sequr_handle, uint8_t* QE, int len) {
    
    long response_code = 0;
    int ret = 0;
    char* full_url = NULL;
    char* res= NULL;
    char* iv_b64 = NULL;
    char* QE_b64 = NULL;
    char qe_len[10];
    uint8_t iv[17]; 
    int32_t iv_len; 
    QP_Handle qp_handle;
    int encoded_QE_len = 0;
    uint8_t *encoded_QE;
    
    if(sequr_handle == NULL || QE == NULL) { return (-1); }
    if(sequr_handle->qp_handle == NULL || sequr_handle->q_meta == NULL || sequr_handle->q_meta->url == NULL || sequr_handle->q_meta->token == NULL) { return (-1);}

    // Perform HTTPS GET request to get QE
    full_url = (char *)calloc(strlen(sequr_handle->q_meta->url) + 80, sizeof(char));
    if (full_url == NULL) { return (-1);}
    strcat(full_url, sequr_handle->q_meta->url);
    strcat(full_url, "/qe/");
    sprintf(qe_len, "%d", len);
    strcat(full_url, qe_len);

    res = QiSpaceAPI_call("GET", full_url, sequr_handle->q_meta->token, "", &response_code);
    free(full_url);
    if (response_code != 200) {
        free(res);
        return -1;
    }
    
    ret = json_get_values(res, NULL, NULL, NULL, &iv_b64, NULL, &QE_b64);
    free(res);
    if (ret != 0) { return (-1);}

    // Create QEEP handle
    qp_handle = sequr_handle->qp_handle;
    if (qp_handle == NULL) { return (-1);}
    
    // decode iv from base64 format to byte array
    iv_len = base64_decode(iv_b64, strlen(iv_b64), iv);
    // load iv
    ret = QP_iv_set(qp_handle, iv, iv_len);
    if (ret != QEEP_OK) { return (-1);}

    encoded_QE = (uint8_t*)malloc(strlen(QE_b64));
    
    // decode QE from base64 format to byte array 
    encoded_QE_len = base64_decode(QE_b64, strlen(QE_b64), encoded_QE);
    // decode QE to plaintext byte array
    ret = QP_decrypt(qp_handle, encoded_QE, encoded_QE_len, QE);    
    if (ret != QEEP_OK) { 
        free(encoded_QE);
        return (-1);
    }

    free(encoded_QE);
    free(iv_b64);
    free(QE_b64);
    return len;
}

/**
  Return the buffer size required by giving key_size in byte and key_type
 */
int32_t sequr_util_key_buff_size(int32_t key_size,  sequr_key_type key_type ) {
    int32_t buff_size = 0;
    switch (key_type) {
        AES_KEY:
            buff_size = key_size + AES_KEY_HEADER_SIZE;
            break;
        QEEP_KEY:
            buff_size = key_size + QEEP_KEY_HEADER_SIZE;
            break;
        QEEP_PASS:
        default:
            buff_size = key_size + QEEP_PASS_HEADER_SIZE;
            break;
    }
    return (buff_size);
}