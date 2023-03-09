package main
/*
#cgo CFLAGS: -I./lib
#cgo LDFLAGS: -L./lib -lqsc
#cgo CFLAGS: -g -Wall 
#include "qispace_sequr.h"
#include "error.h"
*/
import "C"

import (
	"fmt"
	"os"
)

/*
typedef enum {
    QEEP_OK   = 1,
	QEEP_FAIL = 0,
} QEEP_RET

typedef struct QISPACE_SDK_Core_Handle_   *QSC_Handle

*/

const QEEP_OK = 1
// iv size
const IV_SIZE = 16

// default 

func main() {
    fmt.Println("start!")
	/* The pre-loaded variables are in the same format (Base64, Base64, hex string) as if they were retrieved from QiSpace REST API */
	const iv_base64 = "bjH3rL3oGz57YegNTk0avQ=="
	const payload_base64 = "IZzsy4RH+w9YuIc8XsBuZt2Bro2NdngFS32Fds3ftpYSBLAPsUr2695MAhIdBd6xn2t3BAcUNTas4c5k9CXvdgO+WNtuRZTEVewQ3SSm0HdpUzUkoAyjepXj+sLfllOa/EBscA=="
	const sub_key_hex_preload ="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622"

	var qsc_handle_dec C.QSC_Handle
	var ret C.QEEP_RET
	//change string to byte array
	// iv := []byte(iv_base64)
	// seed := []byte(sub_key_hex_preload)

	/** 
	* ##################################################################
	*   Use Sub Key to decode QiSpace API payload  
	*  ##################################################################
	*/
	fmt.Printf("Demo SEQUR decode operation...\n")

	/**
	*    Step 1: Create SEQUR decoder handle
	*/
	fmt.Printf("initializing sequr handle...\n")
	ret := C.QSC_init(&qsc_handle_dec)
	if (ret != C.QEEP_OK) { 
		fmt.Printf("QSC_init fail\n")
		os.Exit(-1)
	}
	fmt.Printf("success!\n")

}