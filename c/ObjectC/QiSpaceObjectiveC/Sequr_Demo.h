//
//  Sequr_Demo.h
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-12.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Sequr_Demo : UIViewController
@property (weak, nonatomic) IBOutlet UILabel *decoded_qe_text;
@property (weak, nonatomic) IBOutlet UILabel *random_string_text;

void build_decoding_table(void);
void base64_cleanup(void);

uint8_t* base64_decode(const char *data, size_t input_length, size_t *output_length);

size_t convert_hex(uint8_t *dest, size_t count, const char *src);
size_t convert_hex_inv(char *dest, size_t count, const uint8_t *src);

extern uint8_t *subkey;
extern size_t subkey_len;

@end

NS_ASSUME_NONNULL_END
