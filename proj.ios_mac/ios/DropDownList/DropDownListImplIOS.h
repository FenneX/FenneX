//
//  DropDownListImplIOS.h
//  FenneX
//
//  Created by Pierre Bertinet on 21/09/15.
//
//

#ifndef __IFeel__DropDownListImplIOS__
#define __IFeel__DropDownListImplIOS__

#import <UIKit/UIKit.h>

@interface DropDownListImplIOS : NSObject<UIPickerViewDelegate, UIPickerViewDataSource, UIGestureRecognizerDelegate>
{
    UIPickerView* pickerView;
    UIImageView* background;
    UILabel* titleView;
    NSArray* pickerValues;
    int _identifier;
    CGFloat rowHeight;
}

- (id) init;
- (void) show;
- (void) hide;
- (void) dealloc;
- (void) setPossibleValues:(NSArray*)values;
- (void) setIdentifier:(int)identifier;
- (void) setTitle:(NSString*)title;
- (void)pickerView:(UIPickerView *)thePickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component;
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component;
- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component;
- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component;
- (void)tapDetected;
@end

#endif /* defined(__IFeel__DropDownListImplIOS__) */
