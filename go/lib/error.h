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

#ifndef _QEEP_SDK_ERROR_H
#define _QEEP_SDK_ERROR_H

# ifdef  __cplusplus
extern "C" {
# endif


/* Define QEEP SDK API Return Code */
typedef enum {
	QEEP_OK   = 1,  /* Success */
	QEEP_FAIL = 0,  /* Fail in general with unknown reasons */
	QEEP_ERR_MEM_Alloc = -1,   /* memory allocation failed*/ 
    QEEP_ERR_API_PARAM_INVALID = -2, /* function input parameter is invalid */ 
	QEEP_ERR_INPUT_NULL_POINTER = -3, /* input parameter is null */
	QEEP_ERR_INPUT_LENGTH_INVALID = -4, /*input paramter length <=0 */
	QEEP_SEED_LENGTH_INVALID = -5,  /*input SEED length < 0 or > 16K bytes*/
	QEEP_IV_LENGTH_INVALID = -6,    /*input IV length < 16  */
	QEEP_ENTROPY_NOT_ENOUGH = -7, 
	QEEP_KEY_VERSION_MISMATCH = -8,
	QEEP_KEY_MAC_FAIL = -9,
	QEEP_KEY_NOT_INIT = -10,
	QEEP_QBC_KEY_SIZE_INVALID = -11, 
	QEEP_QBC_CIPHER_SIZE_INVALID = -12, 
	QEEP_QPK_PUBLIC_KEY_INVALID = -13, 
	QEEP_QPK_KEY_PAIR_INVALID = -14, 
	QEEP_PQRND_FACTOR_INVALID = -15, 
} QEEP_RET;

# ifdef  __cplusplus
}
# endif

#endif  /*_QEEP_SDK_ERROR_H */