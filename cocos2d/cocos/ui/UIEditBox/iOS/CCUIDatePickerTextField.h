//
//  CCUIDatePickerTextField.h
//  cocos2d_libs
//
//  Created by Pierre Bertinet on 06/12/2017.
//

#ifndef CCUIDatePickerTextField_h
#define CCUIDatePickerTextField_h

#import <UIKit/UIKit.h>
#import "ui/UIEditBox/iOS/UITextField+CCUITextInput.h"

#pragma mark - CCUIDatePickerTextField implementation

@interface CCUIDatePickerTextField : UITextField

@property (nonatomic, retain) UIFont *placeholderFont;

@end

#endif /* CCUIDatePickerTextField_h */
