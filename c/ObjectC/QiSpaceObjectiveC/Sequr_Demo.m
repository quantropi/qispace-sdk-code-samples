//
//  Sequr_Demo.m
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-12.
//

#import "Sequr_Demo.h"
#import "sequrObjCWrapper.h"
#import "ViewController.h"
#include "qispace_info.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SUBKEY_BUFFER_SIZE 1300
#define QE_BUFFER_LEN 50
#define RND_LEN 50


@interface Sequr_Demo ()

@end

@implementation Sequr_Demo

/**
 * Decoding any payload from QiSpace RESTful API utilizes SEQUR decode().
 * This demo page demonstrates the process of decoding Qantum Entropy  (or any QiSpace payload) with SEQUR
 * Furthermore, the setup of the subkey and QE beginning from REST API to storage is also demonstrated.
 *
 * Prerequisite: An enterprise device token for authentication when interfacing QiSpace REST API
 *
 * Logical flow:
 * 1. Press 'Setup subkey' to establish and unpack the subkey with QiSpace Enterprise such that QE can be decrypted
 * 2. Press 'Get QE' to retrieve and unpack the Quantum Entropy from QiSpace Enterprise in order to later seed Pseudo-Quantum-Random-Number-Data (PQRND) API
 * 3. Press 'Get PQRND' to generate pseudo quantum random data
 */

@synthesize decoded_qe_text, random_string_text;

// Attributes
// App client and server shoud have the same token and subkey at all times
// Assume token is given for this demo app
// For real applications, the token and subkey are initialize during application setup
uint8_t *subkey = NULL;
size_t subkey_len;
uint8_t *qe;
int qe_len_val;
uint8_t *random_bytes = NULL;
pQrnd_Handle randHandle = NULL;

- (void)viewDidLoad {
    [super viewDidLoad];
    //Setup initializations
    qe = NULL;
    random_bytes = (uint8_t *)calloc(RND_LEN, 1);}

/**
 * Stores subkey from QiSpace Enterprise REST API using device bearer token
 *
 *  @param token the  bearer token for this specified Enterprise device used as authentication to obtain subkey from QiSpace REST API
 *  @param subkey the byte array  where the operable subkey will be stored
 *  @param subkey_len the length of the subkey's byte array
 */
void get_subkey(NSString *token, uint8_t *subkey, size_t *subkey_len){
    NSString *base = [NSString stringWithUTF8String: QISPACE_ENTERPRISE_BASE_URL];
    NSString *subkey_endpoint = [NSString stringWithFormat:@"%@/api/v1/sub_key", base];
    
    // QiSpace REST API Call to obtain subkey
    /**
     *       Sends a POST request to obtain
     *       the subkey, a necessary component to decrypting the payload,
     *       in hex string format.
     */
    NSMutableURLRequest *subkey_request = [[NSMutableURLRequest alloc] initWithURL: [NSURL URLWithString: subkey_endpoint]];
    
    // Configuring QiSpace subkey REST API request with specifications
    [subkey_request setHTTPMethod: @"POST"];
    [subkey_request setValue:@"application/json"
          forHTTPHeaderField:@"Content-Type"];
    [subkey_request setValue:token
          forHTTPHeaderField:@"Authorization"];
    
    NSURLSession *subkey_session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                          delegate:nil
                                                     delegateQueue:[NSOperationQueue mainQueue]];
    
    NSURLSessionDataTask *subkey_task = [subkey_session dataTaskWithRequest:subkey_request
                                    completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        NSHTTPURLResponse *subkey_response = (NSHTTPURLResponse *) response;

        // Verify the HTTP response from QiSpace Enterprise
        if((long)[subkey_response statusCode] != 200){
            NSLog(@"Error with URL RESPONSE");
            return;
        }
        
        // Processing and extracting json payload as C-string
        NSDictionary *subkey_payload = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
        NSString *subkey_string = [subkey_payload objectForKey: @"sub_key"];
        const char *subkey_c_string = [subkey_string UTF8String];

        *subkey_len = strlen(subkey_c_string)/2;
        
        // Convert subkey hex string to byte array
        convert_hex_to_bytes(subkey, strlen(subkey_c_string), subkey_c_string);
    }];
    // Trigger HTTP request to establish subkey from QiSpace Enterprise REST API
    [subkey_task resume];
}


/**
 * Handles 'setup subkey' button press event
 * Prerequisite: the enterprise devicei token must be valid for authentication when interfacing with QiSpace REST API
 */
- (IBAction)event_subkey_pressed:(id)sender {
    if(subkey == NULL){
        subkey = (uint8_t *)calloc(SUBKEY_BUFFER_SIZE, sizeof(uint8_t));
    }
    
    get_subkey(token, subkey, &subkey_len);
}

/**
 *  Unpacks the retrieved QE to a displayable hex stirng format
 *
 * - Prerequisite:
 *  - subkey: must be retrieved before QE is decoded
 * @param data the HTTP request json data from QiSpace Enterprise containing an encrypted Base64 QE and its respective IV
 * @returns a hex string representation of the decrypted QE
 */
char *unpack_qe(NSData *data){
    // Process json as dictionary and extract the Quantum Entropy and its IV
    NSDictionary *qe_payload = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:nil];
    NSString *encoded_qe_string = [qe_payload objectForKey: @"payload"];
    NSString *iv_string = [qe_payload objectForKey: @"iv"];
    
    // Convert to C-string because conversion functions are C-based
    const char *encoded_qe_c_string = [encoded_qe_string UTF8String];
    const char *iv_c_string = [iv_string UTF8String];
    
    // Converting from base64 to byte array format
    size_t iv_len_val;
    uint8_t *encrypted_qe_byte_array = base64_decode_to_bytes(encoded_qe_c_string, strlen(encoded_qe_c_string), &qe_len_val);
    uint8_t *iv_byte_array = base64_decode_to_bytes(iv_c_string, [iv_string length], &iv_len_val);
    
    // Perform SEQUR decoding on QE
    int ret = SEQUR_decode(subkey, subkey_len, iv_byte_array, encrypted_qe_byte_array, qe_len_val, qe);
    
    // Formatting qe result as hex string for output
    return convert_bytes_to_hex(qe, qe_len_val);
}


// Handles 'Get QE' button press event
/** Prerequisites:
 * - subkey:  must be initialized to decode the retrieved QE
 * - token: a proper enterprise device token for authentication when interfacing QiSpace QE REST API
 */
- (IBAction)event_qe_pressed:(id)sender {
    // Validate prerequisites before continuing
    if(subkey == NULL){
        self.decoded_qe_text.text = @"Need to initialize subkey before getting QE";
        return;
    }
    if(qe == NULL){
        qe = (uint8_t *)calloc(QE_BUFFER_LEN, sizeof(uint8_t));
    }
    
    NSString *base = [NSString stringWithUTF8String: QISPACE_ENTERPRISE_BASE_URL];
    NSString *qe_endpoint = [NSString stringWithFormat: @"%@/api/v1/qe/50", base];
    
    /**
     *       QiSpace API response to a GET request consists of
     *       a payload, a pre-encrypted Base64 Quantum Entropy (QE) value,
     *       and an iv, initial vector Base64-encoded, which diminishes data encryption repetition
     */
    // Configuring QiSpace Quantum Entropy REST API request with specifications
    NSMutableURLRequest *qe_request = [[NSMutableURLRequest alloc] initWithURL: [NSURL URLWithString: qe_endpoint]];
    [qe_request setHTTPMethod: @"GET"];
    [qe_request setValue:@"application/json"
          forHTTPHeaderField:@"Content-Type"];
    [qe_request setValue:token
          forHTTPHeaderField:@"Authorization"];
    
    NSURLSession *qe_session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                              delegate:nil
                                        delegateQueue:[NSOperationQueue mainQueue]];
    
    NSURLSessionDataTask *qe_task = [qe_session dataTaskWithRequest:qe_request
                            completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        NSHTTPURLResponse *qe_response = (NSHTTPURLResponse *) response;
        
        // Verify the HTTP response from QiSpace Enterprise
        if((long)[qe_response statusCode] != 200){
            NSLog(@"Error with URL RESPONSE");
        }
        
        // Obtain and output QE from HTTP data
        char *output = unpack_qe(data);
        self.decoded_qe_text.text = [NSString stringWithUTF8String:output];
        
        // Reset PQRND handle
        randHandle = NULL;
    }];
    
    [qe_task resume];
}

/**
 * Returns a pseudo quantum random data string by using QE as seed
 *
 * @param seed a value used to seed PQRND API in order to generate pseudo random data
 * @param seed_len the length of the seed used
 * @param random_bytes the variable holding the PQRND API generated data
 * @param aHandle  an object associated with PQRND API used during setup
 * @returns a hex string representation of the pseudo quantum random data generated by PQRND API
 */
NSString* get_pqrnd(uint8_t *seed, int seed_len, uint8_t *random_bytes, pQrnd_Handle *aHandle){
    int ret;
    // For the first time, seed PQRND with QE on startup
    if(randHandle == NULL){
        ret = pQrnd_setup(seed, seed_len, aHandle);
    }
    
    // Generate PQRND bytes per button press
    ret = pQrnd_generate(*aHandle, random_bytes, RND_LEN);
    
    // Return random bytes as hex
    char *random_str = convert_bytes_to_hex(random_bytes, RND_LEN);
    if(ret != 1){
        return @"An error occurred during PQRND";
    }
    return [NSString stringWithUTF8String: random_str];
    
    // Note: pQrnd will be cleaned up and closed after leaving this page
}

// Handles 'Get PQRND' button press event
- (IBAction)event_pqrnd_pressed:(id)sender {
    //Check that QE has been retrieved so it can be used as seed
    if(qe == NULL){
        random_string_text.text = @"Need to get QE before generating random string";
        return;
    }
    // Gets and displays pseudo quantum random data to screen using QE and pQrnd API handle
    random_string_text.text = get_pqrnd(qe, qe_len_val, random_bytes, &randHandle);
}

// Handles cleanup activities as View exits
-(void)viewWillDisappear:(BOOL)animated
{
    // Cleaning up pQrnd random generation
    if(randHandle != NULL){
        int ret = pQrnd_close(randHandle);
    }
    
    // Freeing allocated memory
    free(qe);
    free(random_bytes);
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
