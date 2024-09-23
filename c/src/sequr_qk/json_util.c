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
#include "cJSON.h"  // Include the cJSON library header

int json_get_values(char* json_str, char** url, char** token, char** subkey, char** iv, char** id, char** qk) {
    // Parse the JSON string
    cJSON *json = cJSON_Parse(json_str);
    if (!json) { 
        return -1; 
    }

    // Extract "url" if requested
    if (url != NULL) {
        cJSON *url_item = cJSON_GetObjectItemCaseSensitive(json, "url");
        if (cJSON_IsString(url_item) && url_item->valuestring != NULL) {
            *url = strdup(url_item->valuestring);
            if (*url == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Extract "token" if requested
    if (token != NULL) {
        cJSON *token_item = cJSON_GetObjectItemCaseSensitive(json, "token");
        if (cJSON_IsString(token_item) && token_item->valuestring != NULL) {
            *token = strdup(token_item->valuestring);
            if (*token == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Extract "sub_key" if requested
    if (subkey != NULL) {
        cJSON *subkey_item = cJSON_GetObjectItemCaseSensitive(json, "sub_key");
        if (cJSON_IsString(subkey_item) && subkey_item->valuestring != NULL) {
            *subkey = strdup(subkey_item->valuestring);
            if (*subkey == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Extract "payload" (qk) if requested
    if (qk != NULL) {
        cJSON *qk_item = cJSON_GetObjectItemCaseSensitive(json, "payload");
        if (cJSON_IsString(qk_item) && qk_item->valuestring != NULL) {
            *qk = strdup(qk_item->valuestring);
            if (*qk == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Extract "iv" if requested
    if (iv != NULL) {
        cJSON *iv_item = cJSON_GetObjectItemCaseSensitive(json, "iv");
        if (cJSON_IsString(iv_item) && iv_item->valuestring != NULL) {
            *iv = strdup(iv_item->valuestring);
            if (*iv == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Extract "id" if requested
    if (id != NULL) {
        cJSON *id_item = cJSON_GetObjectItemCaseSensitive(json, "id");
        if (cJSON_IsString(id_item) && id_item->valuestring != NULL) {
            *id = strdup(id_item->valuestring);
            if (*id == NULL) {
                cJSON_Delete(json);
                return -1;  // Memory allocation failed
            }
        }
    }

    // Clean up and return success
    cJSON_Delete(json);
    return 0;
}