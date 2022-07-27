//
//  ViewController.h
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-12.
//

#import <UIKit/UIKit.h>

void build_decoding_table(void);

void base64_cleanup(void);
uint8_t* base64_decode_to_bytes(const char *data,
                             size_t input_length,
                       size_t *output_length);


//convert hex to byte array
size_t convert_hex_to_bytes(uint8_t *dest, size_t count, const char *src);

//convert byte array to hex
char *convert_bytes_to_hex(uint8_t *arr, int arr_len);

@interface ViewController : UIViewController

extern NSString *token;

@end

