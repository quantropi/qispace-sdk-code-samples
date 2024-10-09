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
#include <ctime>
#include "base64.h"
#ifdef QISPACE_API_ON
#include "RestApiUtil.h"
#include "qispace_sequr.h"
#endif


/*
 * This demo presents a use case of SEQUR API's decode which is used to decode 
 * any arbitrary payload from QiSpace Enterprise's REST API by unpacking and using a sub_key
 * as well as an IV, both pre-loaded in the same format as if they were retrieved 
 * from QiSpace Enterprise.
 * 
 * With QISPACE_API_ON enabled, the sub_key is instead retrieved from QiSpace REST API
 * using function called from RestApiUtil.
 * 
 * It is recommended that this SEQUR API demo is understood first because the same ideas are 
 * used in the PQRND API demo and QEEP API demo
*/

using namespace std;

#define DEMO_SUB_KEY_BUFFER_SIZE           3000
#define DEMO_PAYLOAD_BUFFER_SIZE           3000
#define IV_SIZE                            16



int main(int argc, char **argv)
{
        /* Arbitrary payload and IV that can be retrieve from QiSpace Enterprise */
        std::string iv_base64 = "bjH3rL3oGz57YegNTk0avQ==";
        std::string payload_base64="IZzsy4RH+w9YuIc8XsBuZt2Bro2NdngFS32Fds3ftpYSBLAPsUr2695MAhIdBd6xn2t3BAcUNTas4c5k9CXvdgO+WNtuRZTEVewQ3SSm0HdpUzUkoAyjepXj+sLfllOa/EBscA==";

        std::string sub_key_hex_preload="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622";

        QEEP_RET ret = QEEP_OK;
        int i;

        QSC_Handle qsc_handle_dec;

        uint8_t iv[IV_SIZE]; 
        int32_t iv_len;

        #ifdef QISPACE_API_ON
        std::string sub_key_hex;
        int int_ret = 0;
        #endif

        uint8_t sub_key[DEMO_SUB_KEY_BUFFER_SIZE];
        uint8_t payload[DEMO_PAYLOAD_BUFFER_SIZE];
        int32_t sub_key_len = 0; 
        int32_t payload_len;
        uint8_t plaintext[DEMO_PAYLOAD_BUFFER_SIZE];

/** 
 * ##################################################################
*   Use Sub Key to decode QiSpace API payload  
*  ##################################################################
*/

        std::cout << "Demo SEQUR decode operation...\n";

/**
*    Step 1: Create SEQUR decoder handle
*/
        std::cout << "Initializing SEQUR handle...\n";
        ret = QSC_init(&qsc_handle_dec);
        if (ret != QEEP_OK) { std::cout << "QSC_init fail.\n"; return (-1);}
        std::cout << "Success!\n";

/**
*    Step 2: If QISPACE_API_ON defined, obtain the sub_key from QiSpace Enterprise. 
*       Else, use the pre-loaded sub_key (i.e. a QEEP key as subscriber key). 
*       Load the sub_key into SEQUR decode API
*/
        #ifdef QISPACE_API_ON

                std::cout << "Retrieving subkey from QiSpace Enterprise...\n";
                int_ret = QiSpaceAPI_subKey( sub_key_hex );

                /* If subkey successfully retrieved from QiSpace API, convert from hex string to byte array */
                if (int_ret == 1) {
                        std::cout << "Success!\n"; 
                        sub_key_len = sub_key_hex.length()/2;

                        /* convert retrieved subkey from hex string to byte array */
                        for(i = 0; i < sub_key_len; i++) {
                                sscanf(sub_key_hex.c_str() + 2*i, "%02hhx", &sub_key[i]);
                        }
                }else {
                        std::cout << "QiSpaceAPI_subKey fail: %d \n" << int_ret;
                        return(-1);
                }       
                
        #else
                sub_key_len = sub_key_hex_preload.length()/2;
                /* convert pre-loaded sub_key from hex string to byte array */
                for(i = 0; i< sub_key_len; i++) {
                        sscanf(sub_key_hex_preload.c_str() + 2*i, "%02hhx", &sub_key[i]);
                }
        #endif
        std::cout << "Loading sub key...\n";
        ret = QSC_qeep_key_load(qsc_handle_dec, sub_key, sub_key_len);
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_key_load fail: %d\n" << ret; return (-1);}
        std::cout << "Success!\n";

/**
 *    Step 3: Load payload IV, 
 */
        std::cout << "Loading IV...\n";
        /* decode iv from Base64 string to byte array and load it into SEQUR API */
        iv_len = base64_decode(iv_base64, iv_base64.length(), iv);
        ret = QSC_qeep_iv_set(qsc_handle_dec, iv, iv_len);
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_iv_set fail: %D\n" << ret; return (-1);}
        std::cout << "Success!\n";

/**
 *    Step 4: Decode payload 
 */
        std::cout << "Decoding arbitrary payload...\n";
        /* decode arbitrary payload from Base64 string to byte array and then apply SEQUR decoding */
        payload_len = base64_decode(payload_base64, payload_base64.length(), payload);
        ret = QSC_qeep_decode(qsc_handle_dec, payload,  payload_len, plaintext);
        if (ret != QEEP_OK) { std::cout << "QSC_qeep_decrypt fail: %d\n" << ret; return (-1);}
        std::cout << "Success!\n";

/**
*    Step 5: Release QSC class handle
*/
        ret = QSC_close(qsc_handle_dec);
        std::cout << "Done. \n";

        std::cout << "\n--------------------------------------------------------\n\n";

        return 0;
}