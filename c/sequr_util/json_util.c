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
#include "cjson/cJSON.h"  // Include the cJSON library header

int extract_string_value(cJSON *json, const char *key, char **output) {
    if (output != NULL) {
        cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key);
        if (cJSON_IsString(item) && item->valuestring != NULL) {
            *output = strdup(item->valuestring);
            if (*output == NULL) {
                return -1;  // Memory allocation failed
            }
        }
    }
    return 0;  // Success or no action taken
}

int json_get_values(char* json_str, char** url, char** token, char** subkey, char** iv, char** id, char** qk) {
    // Parse the JSON string
    cJSON *json = cJSON_Parse(json_str);
    if (!json) { 
        return -1; 
    }

    // Use helper function to extract values
    if (extract_string_value(json, "url", url) < 0 ||
        extract_string_value(json, "token", token) < 0 ||
        extract_string_value(json, "sub_key", subkey) < 0 ||
        extract_string_value(json, "payload", qk) < 0 ||
        extract_string_value(json, "iv", iv) < 0 ||
        extract_string_value(json, "id", id) < 0) {
        cJSON_Delete(json);
        return -1;  // Memory allocation failed
    }

    // Clean up and return success
    cJSON_Delete(json);
    return 0;
}