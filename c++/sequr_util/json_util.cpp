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
#include <string> 
#include <cstring>
#include <iostream>
#include <cjson/cJSON.h> 
#include "json_util.h"

using namespace std;

//Extract specific values from a JSON string. 
int json_get_values(const std::string json_str, std::string key, std::string& output) {
    // Parse the JSON string
    auto json = cJSON_Parse(json_str.c_str());
    if (json == nullptr) { 
        return -1; 
    }

    cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key.c_str());
    if (cJSON_IsString(item) && item->valuestring != nullptr) {
        output = item->valuestring;
    }

    cJSON_Delete(json);
    return 0;
}