//
//  Qeep_Demo.h
//  QiSpace_ObjectiveC_Demo
//
//  Created by Roy Zheng on 2022-06-13.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface Qeep_Demo : UIViewController

@property (weak, nonatomic) IBOutlet UITextField *message_field;
@property (weak, nonatomic) IBOutlet UILabel *encrypted_message_text;
@property (weak, nonatomic) IBOutlet UILabel *decrypted_message_text;

@end

NS_ASSUME_NONNULL_END
