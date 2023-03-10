package main
/*
#cgo CFLAGS: -I./lib
#cgo LDFLAGS: -lstdc++ -lm
#cgo LDFLAGS: ./lib/libqeep-x86_64-linux.a
#cgo CFLAGS: -g -Wall 
#include "qispace_sequr.h"
#include "error.h"
*/
import "C"

import (
	"fmt"
	"os"
	"encoding/hex"
	"encoding/base64"
	"unsafe"
	"encoding/json"
	"net/http"
	"io/ioutil"
)

/*
typedef enum {
    QEEP_OK   = 1,
} QEEP_RET

typedef struct QISPACE_SDK_Core_Handle_   *QSC_Handle

*/
type Subkey struct {
	Subkey string `json:"sub_key"`
	KeyIndex  string `json:"key_index"`
}

const IV_SIZE = 16
const DEMO_SUB_KEY_BUFFER_SIZE = 3000
const DEMO_PAYLOAD_BUFFER_SIZE = 3000
const QISPACE_API_ON = true

const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJjbGllbnRfaWQiOjQ4NzksImRldmljZV9pZCI6NDg3OSwidG9rZW5faWQiOjQwOTgsImlhdCI6MTY3ODQ2Mjg0M30.YvGeNro_gd882yjTUffNYewLyCcj2_HKkn0_mV7P6Qk"
const URL = "https://enterprise.staging.qispace.info/kds/api/v1/sub_key"
// default 

func main() {
    fmt.Println("start!")
	/* The pre-loaded variables are in the same format (Base64, Base64, hex string) as if they were retrieved from QiSpace REST API */
	const iv_base64 = "bjH3rL3oGz57YegNTk0avQ=="
	const payload_base64 = "IZzsy4RH+w9YuIc8XsBuZt2Bro2NdngFS32Fds3ftpYSBLAPsUr2695MAhIdBd6xn2t3BAcUNTas4c5k9CXvdgO+WNtuRZTEVewQ3SSm0HdpUzUkoAyjepXj+sLfllOa/EBscA=="
	const sub_key_hex_preload ="01e8e86830efc1c6559d315fd635bd7e59eaa5a0a95100c5d0a2431972ee7f8c0869d47df216794ecf49374b40823071e7836c8f2622"

	var qsc_handle_dec C.QSC_Handle
	var ret C.QEEP_RET
	
	var sub_key_len int
	var iv []byte
	var sub_key []uint8

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
	ret = C.QSC_init(&qsc_handle_dec)
	if (ret != C.QEEP_OK) { 
		fmt.Printf("QSC_init fail\n")
		os.Exit(-1)
	}
	fmt.Printf("success!\n")

	/**
	*    Step 2: If QISPACE_API_ON defined, obtain the sub_key from QiSpace Enterprise. Else, use the pre-loaded sub_key (i.e. a QEEP key as subscriber key). 
	*    Load the sub_key into SEQUR decode API
	*/
	if(QISPACE_API_ON){
		// call 
		fmt.Printf("retrieving subkey from QiSpace Enterprise...\n");
		/* If subkey successfully retrieved from QiSpace API, convert from hex string to byte array */
		// create request
		req, err := http.NewRequest("POST", URL, nil)
		if err != nil {
			panic(err)
		}
		// set headers
		req.Header.Add("Authorization", "Bearer " + token)
		req.Header.Add("Accept", "application/json")

		// send request with headers
		client := &http.Client{}
		resp, err := client.Do(req)
		if err != nil {   
			fmt.Printf("QiSpaceAPI_subKey fail: %s", err)
			os.Exit(-1)
		}
		defer resp.Body.Close()
		body, err := ioutil.ReadAll(resp.Body)

		var post Subkey
		err = json.Unmarshal(body, &post)
		if err != nil {
			fmt.Printf("Reading body failed: %s", err)
			os.Exit(-1)
		}
		sub, err := hex.DecodeString(post.Subkey)
		if err != nil {
			panic(err)
		}
		sub_key_len = len(sub)
		sub_key = make([]uint8, sub_key_len)
		copy(sub_key, sub[:sub_key_len])
	}else{
		sub, err := hex.DecodeString(sub_key_hex_preload)
		if err != nil {
			panic(err)
		}
		sub_key_len = len(sub)
		sub_key = make([]uint8, sub_key_len)
		copy(sub_key, sub[:sub_key_len])
	}
	
	fmt.Printf("loading sub key... \n");
	ret = C.QSC_qeep_key_load(qsc_handle_dec, (*C.uchar)(unsafe.Pointer(&sub_key[0])), C.int(sub_key_len));
	if (ret != C.QEEP_OK) { 
		fmt.Printf("QSC_qeep_key_load fail ret = %d\n", ret)
		os.Exit(-1)
	}
	fmt.Printf("success!\n");

	/**
	*    Step 3: Load payload IV, 
	*/
	fmt.Printf("loading iv...\n");
    /* decode iv from Base64 string to byte array and load it into SEQUR API */
    iv, err := base64.StdEncoding.DecodeString(iv_base64)
	if err != nil {
		panic(err)
	}

    ret = C.QSC_qeep_iv_set(qsc_handle_dec, (*C.uchar)(unsafe.Pointer(&iv[0])), C.int(len(iv)));
    if (ret != C.QEEP_OK) { 
		fmt.Printf("QSC_qeep_iv_set fail\n") 
		os.Exit(-1)
	}
    fmt.Printf("success!\n");

	/**
	*    Step 4: Decode payload 
	*/
	fmt.Printf("decoding arbitrary payload...\n");
	/* decode arbitrary payload from Base64 string to byte array and then apply SEQUR decoding */
	payload, err := base64.StdEncoding.DecodeString(payload_base64)
	if err != nil {
		panic(err)
	}
	payload_len := len(payload)
	plaintext := make([]byte, payload_len)
	ret = C.QSC_qeep_decode(qsc_handle_dec, (*C.uchar)(unsafe.Pointer(&payload[0])), C.int(payload_len), (*C.uchar)(unsafe.Pointer(&plaintext[0])));
	if (ret != C.QEEP_OK) { 
		fmt.Printf("QSC_qeep_decrypt fail\n")
		os.Exit(-1)
	}
	fmt.Printf("success!\n");

	/**
	*    Step 5: Release QSC class handle
	*/
	ret = C.QSC_close(qsc_handle_dec);
	fmt.Printf("Done \n");

	fmt.Printf("\n--------------------------------------------------------\n\n");
}