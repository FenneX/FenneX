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

#import "ImagePicker.h"
#import "UIImage+Resize.h"
#import "AppController.h"
#import "RootViewController.h"
#import <MobileCoreServices/UTCoreTypes.h>
#include "FenneX.h"
#include "ImagePickerWrapper.h"
USING_NS_CC;
USING_NS_FENNEX;

//JPG compression is between 0.0 and 1.0, where 1.0 means best quality.
#define DEFAULT_JPG_COMPRESSION 1.0

void startWidget(UIImagePickerControllerSourceType sourceType)
{
    Director::getInstance()->stopAnimation();
    [[ImagePicker sharedPicker] initController];
    [[ImagePicker sharedPicker] setSourceType:sourceType];
    if([AppController sharedController] != nullptr)
    {
        if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad && sourceType == UIImagePickerControllerSourceTypePhotoLibrary)
        {
            UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:[ImagePicker sharedPicker].controller];
            [popover setDelegate:[ImagePicker sharedPicker]];
            //Tested on iPad retina and not retina, will show the popover on the bottom right corner with an Arrow Up.
            CGRect rect = CGRectMake(0, 0, 2048, 250);
            if(UIInterfaceOrientationIsPortrait([[UIApplication sharedApplication] statusBarOrientation]))
            {
                rect.size.width /= 2;
            }
            //Doesn't work well with Left since iOS8.
            [popover presentPopoverFromRect:rect inView:[AppController sharedController].viewController.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
            [ImagePicker sharedPicker].popOver = popover;
        }
        else
        {
            [[AppController sharedController].viewController presentViewController:[ImagePicker sharedPicker].controller animated:YES completion:nil];
        }
    }
}

void pickImageWithWidget()
{
    startWidget(UIImagePickerControllerSourceTypePhotoLibrary);
    
}
void takePhotoWithWidget()
{
    startWidget(UIImagePickerControllerSourceTypeCamera);
    
}

bool isCameraAvailable()
{
    return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera];
}

@implementation ImagePicker

@synthesize controller;
@synthesize popOver;
static ImagePicker* _sharedPicker = nil;

+ (ImagePicker*) sharedPicker
{
	@synchronized([ImagePicker class])
	{
		if (!_sharedPicker)
			[[self alloc] init];
		
		return _sharedPicker;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([ImagePicker class])
	{
		NSAssert(_sharedPicker == nil, @"Attempted to allocate a second instance of ImagePicker singleton.");
		_sharedPicker = [super alloc];
		return _sharedPicker;
	}
	
	return nil;
}

- (id) init
{
	self = [super init];
	if (self != nil)
	{
        // Set up the image picker controller
    }
    return self;
}

- (void) setSourceType:(UIImagePickerControllerSourceType)sourceType
{
    controller.sourceType = sourceType;
}

- (void) initController
{
    [controller release];
    controller = [[UIImagePickerController alloc] init];
    controller.delegate = self;
    controller.allowsEditing = false;
}

// For responding to the user accepting a newly-captured picture or movie
- (void) imagePickerController:(UIImagePickerController*) picker didFinishPickingMediaWithInfo:(NSDictionary*) info
{
    NSString *mediaType = [info objectForKey: UIImagePickerControllerMediaType];
    UIImage *originalImage, *editedImage, *imageToSave;
    
    if (CFStringCompare ((CFStringRef) mediaType, kUTTypeImage, 0) == kCFCompareEqualTo)
    {
        editedImage = (UIImage *) [info objectForKey:UIImagePickerControllerEditedImage];
        originalImage = (UIImage *) [info objectForKey:UIImagePickerControllerOriginalImage];

        imageToSave = editedImage != nil ? editedImage : originalImage;
        
        // Save the new image (original or edited) to the Camera Roll  if the image have metadata, which means it comes from the library
        if([info objectForKey: UIImagePickerControllerMediaMetadata] != nil)
        {
            NSLog(@"Saving photo in library");
            UIImageWriteToSavedPhotosAlbum (imageToSave, nil, nil , nil);
        }
        
        //To convert to CCSprite : http://www.cocos2d-x.org/boards/6/topics/3922
        NSLog(@"Original Image size : %f, %f", imageToSave.size.width, imageToSave.size.height);
        
        //We need to save the image to a temporary file: Cocos2d-x is not able to read the file directly from an assets-library:// url, which is the only type we can get from ALAssetsLibrary
        NSString * filename = [NSString stringWithFormat: @"%f.jpg",[[NSDate date]timeIntervalSinceReferenceDate]];
        NSString * tmpfile = [NSTemporaryDirectory() stringByAppendingPathComponent:filename];
        
        NSError* error = nullptr;
        BOOL result = [UIImageJPEGRepresentation(imageToSave, DEFAULT_JPG_COMPRESSION) writeToFile:tmpfile options:NSDataWritingAtomic error:&error];
        NSLog(@"Write result for file %@ : %@, full path: %@", filename, (result ? @"OK" : @"Problem"), tmpfile);
        if(result)
        {
            Director::getInstance()->getTextureCache()->removeTextureForKey([tmpfile UTF8String]);
            notifyImagePicked([tmpfile UTF8String]);
        }
        else
        {
            NSLog(@"Write error description: %@, reason: %@", [error localizedDescription], [error localizedFailureReason]);
            notifyImagePickCancelled();
        }
        
        controller.view.hidden = YES;
        controller = [[UIImagePickerController alloc] init];
        controller.delegate = self;
    }
    else
    {
        NSLog(@"Problem : picked a media which is not an image");
        notifyImagePickCancelled();
    }
    [picker dismissViewControllerAnimated:YES completion:nil];
    Director::getInstance()->startAnimation();
    if(popOver)
    {
        [popOver dismissPopoverAnimated:YES];
    }
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
    [self imagePickerControllerDidCancel:controller];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    // Dismiss the image selection and close the program
    [picker dismissViewControllerAnimated:YES completion:nil];
    Director::getInstance()->startAnimation();
    if(popOver)
    {
        [popOver dismissPopoverAnimated:YES];
    }
    
    notifyImagePickCancelled();
}

@end
