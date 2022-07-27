//
//  ViewController.m
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-12.
//

#import "ViewController.h"
#import "Sequr_Demo.h"
#import "Qeep_Demo.h"
#import "Masq_Demo.h"
#include "qispace_info.h"

/**
 * Redirects user to each of the following demos to explore SEQUR, QEEP and eventually MASQ functionality:
 *  - Sequr_Demo: Demonstrates subkey setup process (retrieval and unpacking) which will normally be done on device setup, QE retrieval and unpacking with SEQUR API, and PQRND API pseudo quantum random data generation
 *  - Qeep_Demo: Demonstrates QK retrieval from QiSpace Enterprise, decoding QK with SEQUR API and encryption/decryption with QEEP API
 *
 *  Note: Setup subkey in Sequr_Demo should be executed before Qeep_Demo since subkey is initialized with an outdated subkey
 */

// Base64 decoding then convert to bytes

NSString *token = @"";

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
void build_decoding_table(void) {

    decoding_table = malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

void base64_cleanup(void) {
    if(decoding_table != NULL){
        free(decoding_table);
    }
}
uint8_t* base64_decode_to_bytes(const char *data,
                             size_t input_length,
                             size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    uint8_t *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
    
    return decoded_data;
}


// Convert hex to byte array
size_t convert_hex_to_bytes(uint8_t *dest, size_t count, const char *src) {
    char buf[3];
    size_t i;
    int value;
    for (i = 0; i < count && *src; i++) {
        buf[0] = *src++;
        buf[1] = '\0';
        if (*src) {
            buf[1] = *src++;
            buf[2] = '\0';
        }
        if (sscanf(buf, "%x", &value) != 1)
            break;
        dest[i] = value;
    }
    return i;
}

// convert byte array to hex
char *convert_bytes_to_hex(uint8_t *arr, int arr_len){
    const size_t hexlen = 2;
    const int outstrlen = arr_len * hexlen;
    
    char *outstr = malloc(outstrlen+1);
    char *p = outstr;
    for(size_t i = 0;i<arr_len;i++){
        p+=sprintf(p, "%.2x", arr[i]);
    }
    return outstr;
}

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSString *access = [NSString stringWithUTF8String:QISPACE_ENTERPRISE_ACCESS_TOKEN];
    token = [NSString stringWithFormat:@"Bearer %@", access];
}
- (IBAction)sequr_demo_pressed:(id)sender {
    Sequr_Demo * nvc = [[Sequr_Demo alloc] initWithNibName: nil bundle: nil];
    [self presentViewController:nvc animated:YES completion:nil];
}

- (IBAction)qeep_demo_pressed:(id)sender {
    Qeep_Demo *nvc = [[Qeep_Demo alloc] initWithNibName: nil bundle: nil];
    [self presentViewController:nvc animated:YES completion:nil];
}
- (IBAction)masq_demo_pressed:(id)sender {
    Masq_Demo *nvc = [[Masq_Demo alloc] initWithNibName: nil bundle: nil];
    [self presentViewController:nvc animated:YES completion:nil];
}

@end
