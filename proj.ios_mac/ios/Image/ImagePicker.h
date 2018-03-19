/****************************************************************************
 Copyright (c) 2013-2014 Auticiel SAS
 
 http://www.fennex.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************///

#import <UIKit/UIKit.h>
#include "FileUtility.h"

@interface ImagePicker : NSObject <UINavigationControllerDelegate, UIImagePickerControllerDelegate, UIPopoverControllerDelegate>
{
    UIImagePickerController* controller;
    NSString* saveName;
    FenneX::FileLocation saveLocation;
    NSString* identifier;
    int width;
    int height;
    float thumbnailScale;
    UIPopoverController* popOver;
    bool rescale;
}

@property (nonatomic, readonly) UIImagePickerController* controller;
@property (nonatomic, retain) NSString* saveName;
@property (nonatomic, assign) FenneX::FileLocation saveLocation;
@property (nonatomic, retain) NSString* identifier;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) float thumbnailScale;
@property (nonatomic, assign) bool rescale;
@property (nonatomic, assign) UIPopoverController* popOver;

- (void) initController;
- (void) setSourceType:(UIImagePickerControllerSourceType)sourceType;

- (void) imagePickerController:(UIImagePickerController*) picker didFinishPickingMediaWithInfo:(NSDictionary*) info;
- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker;

+ (ImagePicker*) sharedPicker;


@end
