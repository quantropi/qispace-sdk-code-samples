//
//  Qeep_Demo.m
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-13.
//

#import "Qeep_Demo.h"
#import "qeepObjCWrapper.h"
#import "Sequr_Demo.h"
#import "ViewController.h"
#import "sequrObjCWrapper.h"
#include "qispace_info.h"

/**
 * This page demonstrates a use of the QEEP API to encrypt and dedcrypt a message with a QiSpace retrieved Quantum Key (QK) and iv.
 * Also, the SEQUR API is also demonstrated as a by-product of decoding the retrieved Quantum Key (QK) and iv.
 *
 * Prerequisites:
 *  - An enterprise device token which will be used for QK retrieval from QiSpace Enterprise REST API
 *  - A subkey intended for SEQUR decoding the QK after its retrieval. Realistically, subkey would be initialized on device setup
 *
 *  **Ideally, setting up the subkey from Sequr_Demo must be done before getting QK as QK decryption requires subkey to be initialized
 *
 *  Logical Flow:
 *   1. Press 'Get QK' to create and unpack a Quantum Key from QiSpace Enterprise
 *   2. Enter a message into the text field
 *   3. Press 'Encrypt' to QEEP encrypt the text field message
 *   4. Press 'Decrypt' to QEEP decrypt the encrypted text field message
 */
// Note: Assume subkey is given for this demo page, so it must be retrieved with on the SEQUR page. Realistically, it would be initialized on device setup



// Setup macros for constants
#define QK_KEY_LENGTH 64
#define QK_BUFFER_SIZE (QK_KEY_LENGTH+32)
#define MESSAGE_IV_LENGTH 16

// Helper function to convert string to byte array
void string2ByteArray(char* input, uint8_t* output)
{
    int loop = 0;
    int i = 0;
    while(input[loop] != '\0'){
        output[i++] = input[loop++];
    }
}



@interface Qeep_Demo ()
@end

@implementation Qeep_Demo



uint8_t qk[QK_BUFFER_SIZE];
size_t qk_len;
size_t iv_len;
bool qk_retrieved;


// Currently, max message length is 64
uint8_t *encrypted_message;
uint8_t *decrypted_message;

uint8_t *message_iv;        // Note: Must be 16 bytes
NSInteger message_field_len;

@synthesize message_field, encrypted_message_text, decrypted_message_text;



- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Initial setup
    qk_retrieved = false;
    encrypted_message = NULL;
    decrypted_message = NULL;
    
    message_iv = (uint8_t *)calloc(MESSAGE_IV_LENGTH, 1);      // Note: The message iv must be 16 bytes
}



/**
 * Performs base64 decoding, SEQUR decode and stores the QK and IV from HTTP request payload
 *
 *@param data the json data returned from QiSpace Enterprise after issuing a POST request for QK
 *@param response the default HTTP response regarding the QK request
 */
void unpack_qk(NSData * _Nullable data, NSURLResponse * _Nullable response){
    NSHTTPURLResponse *qk_response = (NSHTTPURLResponse *) response;
    NSLog(@"The response is: %@", qk_response);

    // Validate QiSpace API payload
    if((long)[qk_response statusCode] != 200){
        NSLog(@"Error with URL RESPONSE");
        return;
    }
    
    // Process qk json data as dictionary and extract QK and its IV
    NSDictionary *qk_payload = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
    NSLog(@"object: %@ \n", qk_payload);
    NSString *encrypted_qk_string = [qk_payload objectForKey: @"payload"];
    NSString *encrypted_qk_iv_string = [qk_payload objectForKey: @"iv"];
    
    // Convert to C-string because Base64 conversion is C-based
    const char *encrypted_qk_c_string = [encrypted_qk_string UTF8String];
    const char *encrypted_qk_iv_c_string = [encrypted_qk_iv_string UTF8String];
    
    // Converting QK and iv from Base64 to byte array
    uint8_t *unpacked_qk_byte_array = base64_decode_to_bytes(encrypted_qk_c_string, strlen(encrypted_qk_c_string), &qk_len);
    uint8_t *iv_byte_array = base64_decode_to_bytes(encrypted_qk_iv_c_string, strlen(encrypted_qk_iv_c_string), &iv_len);
   
    // SEQUR decode the retrieved QK using subkey
    memset(qk, 0, QK_BUFFER_SIZE);
    int ret = SEQUR_decode(subkey, subkey_len, iv_byte_array, unpacked_qk_byte_array, qk_len, qk);
    
    qk_retrieved = true;
}

/**
 * Triggers retrieval of QK from the QiSpace Enterprise REST API
 *
 * @param token the enterprise device bearer token used as authentication when accessing QiSpace API
 */
void get_qk(NSString *token){
    
    NSString *base = [NSString stringWithUTF8String:QISPACE_ENTERPRISE_BASE_URL];
    NSString *qk_endpoint = [NSString stringWithFormat:@"%@/api/v1/qk/", base];
    
    /**
     *       QISpace API response consists of
     *       a payload, a pre-encrypted Qeep Key (QK),
     *       and an iv, initialization vector to reduce encryption repetition,
     *       all in Base64 format
     */
    // Configuring QiSpace Quantum Key (QK) REST API request with specifications
    NSMutableURLRequest *qk_request = [[NSMutableURLRequest alloc] initWithURL: [NSURL URLWithString: qk_endpoint]];
    [qk_request setHTTPMethod: @"POST"];
    [qk_request setValue:@"application/json"
          forHTTPHeaderField:@"Content-Type"];
    [qk_request setValue:token
          forHTTPHeaderField:@"Authorization"];
    
    NSDictionary *qk_param_dict = @{@"key_length":@QK_KEY_LENGTH, @"label":@"demo-device"};
    NSData *qk_param = [NSJSONSerialization dataWithJSONObject:qk_param_dict options:kNilOptions error:nil];
    [qk_request setHTTPBody:qk_param];
    
    NSURLSession *qk_session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                          delegate:nil
                                                     delegateQueue:[NSOperationQueue mainQueue]];
    
    NSURLSessionDataTask *qk_task = [qk_session dataTaskWithRequest:qk_request
                            completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        unpack_qk(data, response);
    }];
    // Trigger QiSpace QK request API call
    [qk_task resume];
}




/**
 * Handles 'Get QK" button press event
 * 
 * Prerequisite: requires sub_key to be setup so that retrieved QK can be decoded
 */
- (IBAction)event_qk_pressed:(id)sender {
    // Assert subkey is initialized before continuing
    if(subkey == NULL){
        encrypted_message_text.text = @"Need to initialize subkey before getting QK";
        decrypted_message_text.text = @"Need to initialize subkey before getting QK";
        return;
    }
    get_qk(token);
}

/**
 * Generates an IV and encrypts the message using QEEP API with the retrieved QK
 *
 * @param qk the Quantum Key for symmetric encryption used in QEEP API
 * @param qk_len length value of the Quantum Key (QK)
 * @param message_iv byte array  where the message's initialization vector will be stored
 * @param message_field_len length value of the text field message
 * @param encrypted_message the variable designed to store the QEEP encrypted message
 * @param message_field_string the string to be QEEP encrypted
 * @returns the encrypted message as a UTF-8 string  or an error statement
 */
NSString* encrypt_operation(uint8_t *qk, size_t qk_len, uint8_t *message_iv, NSInteger message_field_len, uint8_t *encrypted_message, NSString *message_field_string){
    // Generate random 16 bytes (MANDATORY) iv per encrypt operation or per message
    // The message iv is simply fixed for this demo
    char *message_iv_as_hex = "0102030405060708090a010203040506";
    
    // Convert message iv from hex to byte array
    convert_hex_to_bytes(message_iv, strlen(message_iv_as_hex), message_iv_as_hex);
    
    // Dynamically allocate memory for encrypted
    encrypted_message = (uint8_t*)calloc(message_field_len, sizeof(uint8_t));
    
    // Convert the message to raw byte array
    uint8_t *raw_message = (uint8_t*)calloc(message_field_len, 1);
    string2ByteArray([message_field_string UTF8String], raw_message);
    
    // QEEP encrypt the text field message with the retrieved QK and generated IV
    int ret = QEEP_encrypt(qk, qk_len, message_iv, raw_message, (int32_t)message_field_len, encrypted_message);
    
    // Handle UI display with error codes
    NSString *output;
    if(ret == 1){
        // Formatting byte array into hex string
        char *outstr = convert_bytes_to_hex(encrypted_message, (int)message_field_len);
        output = [NSString stringWithUTF8String: outstr];
        free(outstr);
    }
    else{
        output = [NSString stringWithFormat: @"Error code: %d occurred in encryption. Consult error.h for more information", ret];
    }
    
    // Perform cleanup by freeing allocated memory
    free(raw_message);
    free(encrypted_message);
    
    return output;
}


/**
 * Handles 'Encrypt' button press event
 * Prerequisite: QK must exist as it is used as the symmetric encryption key
 */
- (IBAction)event_encrypt_pressed:(id)sender {
    // Check qk initialized for QEEP encryption/decryption requires QK
    if(!qk_retrieved){
        encrypted_message_text.text = @"Need to retrieve qk before encrypting";
        return;
    }
    
    // Get message from input field
    NSString *message_field_string = message_field.text;
    message_field_len = [message_field_string length];
    
    // Encrypt the message with QK and message_iv
    encrypted_message_text.text = encrypt_operation(qk, qk_len, message_iv, message_field_len, encrypted_message, message_field_string);
}

/**
 * Applies QEEP decryption on the encrypted message
 *
 * @param qk the Quantum Key used for symmetric cryptography with QEEP
 * @param qk_len the length value of the Quantum Key
 * @param message_iv the same initialization vector used to encrypt the message
 * @param encrypted_message_string the message to be QEEP decrypted in string form
 * @param message_field_len the length of the original message/decrypted message
 * @param decrypted_message the variable to store the QEEP decrypted message
 * @returns the decrypted message as utf-8 string or an error statement
 */
NSString* decrypt_operation(uint8_t *qk, size_t qk_len, uint8_t *message_iv, NSString *encrypted_message_string, NSInteger message_field_len, uint8_t *decrypted_message){
    decrypted_message = (uint8_t*)calloc(message_field_len, sizeof(uint8_t));
    
    // Convert char* to byte array
    const char *text = [encrypted_message_string UTF8String];
    uint8_t *encr_msg = (uint8_t *)calloc(message_field_len, sizeof(uint8_t));
    convert_hex_to_bytes(encr_msg, strlen(text), text);
    
    printf("encr_msg: %s\n", encr_msg);
    
    // Use Qeep API's QEEP_decrypt to decrypt the encrypted message
    // Message iv should be identical to one used during encryption
    int ret = QEEP_decrypt(qk, qk_len, message_iv, encr_msg, message_field_len, decrypted_message);
    
    // Handle UI display depending on error codes
    NSString *output;
    if(ret == 1){
        // Formatting the byte array into a string for printing
        char outstr[message_field_len];
        memcpy(outstr, decrypted_message, message_field_len);
        outstr[message_field_len] = '\0';
        output = [NSString stringWithUTF8String: outstr];
    }
    else{
        output = [NSString stringWithFormat: @"Error code: %d occurred in encryption. Consult error.h for more information", ret];
    }
    
    // Clean up by freeing memory
    free(encr_msg);
    free(decrypted_message);
    
    return output;
}


/**
 * Handles 'Decrypt' button press event
 * Prerequisite: an encrypted message must be present such that there is something to decrypt (also implies QK must be retrieved)    
 */
- (IBAction)event_decrypt_pressed:(id)sender {
    if(!qk_retrieved){
        decrypted_message_text.text = @"Need to retrieve QK before decrypting";
        return;
    }
    
    // Ensure message has been encrypted before attempting decryption
    if([encrypted_message_text.text isEqualToString: @"**encrypted message**"]){
        decrypted_message_text.text = @"Need an encrypted message to decrypt";
        return;
    }
    
    decrypted_message_text.text = decrypt_operation(qk, qk_len, message_iv, encrypted_message_text.text, message_field_len, decrypted_message);
}
// Handles cleanup activities as View exits
- (void)viewWillDisappear:(BOOL)animated{
    qk_retrieved = false;
    NSLog(@"freeing message_iv");
    free(message_iv);
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
