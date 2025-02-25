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

#ifndef _QISPACE_MASQ_DS_ERROR_H
#define _QISPACE_MASQ_DS_ERROR_H

# ifdef  __cplusplus
extern "C" {
# endif

/* QiSpace MASQ DS API Return Code */
typedef enum {
	MASQ_DS_SUCCESS   = 0,      					/* Success */ 
	MASQ_DS_RANDOM_OK = 1,							/* random function success */
	MASQ_DS_ERROR     = -50,      					/* fail in general with unknown error */
	MASQ_DS_ERR_MEM_ALLOC = -1,   					/* memory allocation failed */
	MASQ_DS_ERR_INPUT_NULL_POINTER = -2, 			/* input parameter is null */
	MASQ_DS_ERR_SEED_LENGTH_INVALID = -3,   		/* input seed length <= 0 */ 
	MASQ_DS_ERR_GEN_SEED_FAILED = -4,  				/* failed to generate seed */ 
	MASQ_DS_ERR_GEN_RANDOM_FAILED = -5,  			/* failed to generate random numbers */ 
	MASQ_DS_ERR_WRONG_KEY = -6,  					/* Generated Key is not correct */ 
	MASQ_DS_ERR_WRONG_SIGLEN = -7,  				/* signature length is not correct */ 
	MASQ_DS_ERR_MSG_LENGTH_INVALID = -8,  			/* input message length <= 0 */ 
	MASQ_DS_ERR_SIG_LENGTH_INVALID = -9,   			/* input signature length <= 0 */
	MASQ_DS_ERR_FAILED_VERIFY = -10, 				/* verification failed */ 
} MASQ_DS_API;

# ifdef  __cplusplus
}
# endif

#endif  /*_QISPACE_MASQ_DS_ERROR_H */
