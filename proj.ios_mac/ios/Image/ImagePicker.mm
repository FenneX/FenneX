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

bool pickImageFrom(const std::string& saveName, PickOption pickOption, int width, int height, const std::string& identifier, bool rescale, float thumbnailScale)
{
    Director::getInstance()->stopAnimation();
    [ImagePicker sharedPicker].saveName = [NSString stringWithFormat:@"%s", saveName.c_str()];
    [ImagePicker sharedPicker].identifier = [NSString stringWithFormat:@"%s", identifier.c_str()];
    [ImagePicker sharedPicker].width = width;
    [ImagePicker sharedPicker].height = height;
    [ImagePicker sharedPicker].thumbnailScale = thumbnailScale;
    [ImagePicker sharedPicker].rescale = rescale;
    [[ImagePicker sharedPicker] initController];
    NSLog(@"Picking image %s", saveName.c_str());
    
    [[ImagePicker sharedPicker] setSourceType:pickOption == PickOption::Camera ? UIImagePickerControllerSourceTypeCamera : UIImagePickerControllerSourceTypePhotoLibrary];
    if([AppController sharedController] != NULL)
    {
        if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad && !(pickOption == PickOption::Camera))
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
            [[AppController sharedController].viewController presentModalViewController:[ImagePicker sharedPicker].controller animated:YES];
        }
        //[sharedController.window addSubview:[ImagePicker sharedPicker].controller.view];
        return true;
    }
    return false;
}

bool isCameraAvailable()
{
    return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera];
}

@implementation ImagePicker

@synthesize controller;
@synthesize saveName;
@synthesize identifier;
@synthesize width;
@synthesize height;
@synthesize thumbnailScale;
@synthesize rescale;
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
    controller.allowsEditing = rescale;
}

// For responding to the user accepting a newly-captured picture or movie
- (void) imagePickerController:(UIImagePickerController*) picker didFinishPickingMediaWithInfo:(NSDictionary*) info
{
    NSLog(@"Image picked, save name : %@", saveName);
    NSString *mediaType = [info objectForKey: UIImagePickerControllerMediaType];
    UIImage *originalImage, *editedImage, *imageToSave;
    bool notified = false;
    
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
        UIImage* resultImage = nil;
        
        if(imageToSave.size.width != imageToSave.size.height && rescale)
        {
            float difference = std::abs(imageToSave.size.width - imageToSave.size.height);
            BOOL cropWidth = imageToSave.size.width > imageToSave.size.height;
            NSLog(@"Crop rect : %f, %f, %f, %f", cropWidth ? difference / 2 : 0,
                  !cropWidth ? difference / 2 : 0,
                  cropWidth ? imageToSave.size.width - difference : imageToSave.size.width,
                  !cropWidth ? imageToSave.size.height - difference : imageToSave.size.height);
            int roundingError = (int)difference % 2;
            imageToSave = [imageToSave croppedImage:CGRectMake(cropWidth ? difference / 2 : 0,
                                                               !cropWidth ? difference / 2 : 0,
                                                               cropWidth ? imageToSave.size.width - difference - roundingError : imageToSave.size.width,
                                                               !cropWidth ? imageToSave.size.height - difference - roundingError : imageToSave.size.height)];
            NSLog(@"Image size after crop : %f, %f", imageToSave.size.width, imageToSave.size.height);
        }
        
        CGSize targetSize = imageToSave.size;
        float scaleX = width/imageToSave.size.width;
        float scaleY = height/imageToSave.size.height;
        float scale = MIN(scaleX, scaleY);
        if(scale != 1)
        {
            targetSize.width *= scale;
            targetSize.height *= scale;
            //hard resize because the image can be way too heavy (1536x2048 on 3GS ....)
            resultImage = [imageToSave resizedImage:targetSize interpolationQuality:kCGInterpolationHigh];
            NSLog(@"Image size after resize : %f, %f", resultImage.size.width, resultImage.size.height);
        }
        else
        {
            resultImage = imageToSave;
        }
        controller.view.hidden = YES;
        NSString* fileName = [NSString stringWithFormat:@"Documents/%@.png", saveName];
        NSString* pngPath = [NSHomeDirectory() stringByAppendingPathComponent:fileName];
        NSError* error = NULL;
        BOOL result = [UIImagePNGRepresentation(resultImage) writeToFile:pngPath options:NSDataWritingAtomic error:&error];
        //BOOL result = [UIImagePNGRepresentation(resultImage) writeToFile:pngPath atomically:YES];
        NSLog(@"Write result for file %@ : %@, fullPath : %@", fileName, (result ? @"OK" : @"Problem"), pngPath);
        if(result)
        {
            std::string fullPath = std::string(getenv("HOME")) + "/Documents/" + [saveName UTF8String] + ".png" ;
            Director::getInstance()->getTextureCache()->removeTextureForKey(fullPath.c_str());
            if(thumbnailScale > 0)
            {
                targetSize.width *= thumbnailScale;
                targetSize.height *= thumbnailScale;
                UIImage* thumbnail = [resultImage resizedImage:targetSize interpolationQuality:kCGInterpolationHigh];
                
                NSString* thumbnailName = [NSString stringWithFormat:@"Documents/%@-thumbnail.png", saveName];
                NSString* thumbnailPath = [NSHomeDirectory() stringByAppendingPathComponent:thumbnailName];
                BOOL result = [UIImagePNGRepresentation(thumbnail) writeToFile:thumbnailPath options:NSDataWritingAtomic error:&error];
                NSLog(@"Write result for thumbnail %@ : %@, fullPath : %@", thumbnailName, (result ? @"OK" : @"Problem"), thumbnailPath);
                if(result)
                {
                    std::string fullPathThumbnail = std::string(getenv("HOME")) + "/Documents/" + [saveName UTF8String] + "-thumbnail.png";
                    Director::getInstance()->getTextureCache()->removeTextureForKey(fullPathThumbnail.c_str());
                }
                else
                {
                     NSLog(@"Write error for thumbnail description: %@, reason: %@", [error localizedDescription], [error localizedFailureReason]);
                }
            }
            notifyImagePicked([saveName UTF8String], [identifier UTF8String]);
            notified = true;
        }
        else
        {
            NSLog(@"Write error description: %@, reason: %@", [error localizedDescription], [error localizedFailureReason]);
            //TODO : add a notification for failed save
        }
        
        controller = [[UIImagePickerController alloc] init];
        controller.delegate = self;
    }
    else
    {
        NSLog(@"Problem : picked a media which is not an image");
    }
    [picker dismissModalViewControllerAnimated:YES];
    Director::getInstance()->startAnimation();
    if(popOver)
    {
        [popOver dismissPopoverAnimated:YES];
    }
    if(!notified) notifyImagePickCancelled();
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
    [self imagePickerControllerDidCancel:controller];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    // Dismiss the image selection and close the program
    [picker dismissModalViewControllerAnimated:YES];
    Director::getInstance()->startAnimation();
    if(popOver)
    {
        [popOver dismissPopoverAnimated:YES];
    }
    
    notifyImagePickCancelled();
}

@end
