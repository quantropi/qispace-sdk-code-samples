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

#ifndef _QISPACE_MASQ_KEM_ERROR_H
#define _QISPACE_MASQ_KEM_ERROR_H

# ifdef  __cplusplus
extern "C" {
# endif


/* QiSpace MASQ SDK API Return Code */
typedef enum {
	MASQ_SUCCESS   = 0,      			/* Success */
	MASQ_RANDOM_OK = 1, 				/* random function success */
	MASQ_ERROR     = 10,      			/* fail in general with unknown error */
	MASQ_ERR_MEM_ALLOC = -1,   			/* memory allocation failed */
	MASQ_ERR_INPUT_NULL_POINTER = -2, 	/* input parameter is null */
	MASQ_ERR_INPUT_PARAM_INVALID = -3, 	/* input parameter is invalid */
	MASQ_ERR_SEED_LENGTH_INVALID = -5,  /* input seed length <= 0 */ 
	MASQ_ERR_SEED_FAILED = -6,  		/* failed to generate seed */  
	MASQ_ERR_RANDOM_FAILED = -7  		/* failed to generate random numbers */  
} MASQ_API;

# ifdef  __cplusplus
}
# endif

#endif  /*_QISPACE_MASQ_KEM_ERROR_H */