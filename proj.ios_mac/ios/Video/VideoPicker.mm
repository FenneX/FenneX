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

#import "VideoPicker.h"
#import "AppController.h"
#import "RootViewController.h"
#import <MobileCoreServices/UTCoreTypes.h>
#include "FenneX.h"
#include "VideoPickerWrapper.h"
#include "VideoRecorder.h"
#include "VideoPlayer.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import "VideoPlayerImplIOS.h"
USING_NS_CC;
USING_NS_FENNEX;

void startVideoRecordPreview(Vec2 position, cocos2d::Size size)
{
    [[VideoRecorder sharedRecorder] setPreviewPosition:CGPointMake(position.x, position.y) size:CGSizeMake(size.width, size.height)];
    [[VideoRecorder sharedRecorder] startPreview];
}

void stopVideoRecordPreview()
{
    [[VideoRecorder sharedRecorder] stopPreview];    
}

void startVideoRecording()
{
    [[VideoRecorder sharedRecorder] startRecord];
}

void stopVideoRecording()
{
    [[VideoRecorder sharedRecorder] stopRecord];
}

bool cancelRecording(bool notify)
{
    return [[VideoRecorder sharedRecorder] cancelRecording:notify];
}

bool pickVideoFromLibrary(const std::string& saveName)
{
    if([AppController sharedController] != NULL)
    {
        [[VideoPicker sharedPicker] initController];
        [VideoPicker sharedPicker].saveName = [NSString stringWithFormat:@"%s", saveName.c_str()];
        [[VideoPicker sharedPicker] setSourceType:UIImagePickerControllerSourceTypePhotoLibrary];
        [VideoPicker sharedPicker].useCamera = false;
        if([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad)
        {
            UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:[VideoPicker sharedPicker].controller];
            //Tested on iPad retina and not retina, will show the popover on the bottom right corner with an Arrow Up.
            CGRect rect = CGRectMake(0, 0, 2048, 250);
            if(UIInterfaceOrientationIsPortrait([[UIApplication sharedApplication] statusBarOrientation]))
            {
                rect.size.width /= 2;
            }
            [popover presentPopoverFromRect:rect inView:[AppController sharedController].viewController.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
            [VideoPicker sharedPicker].popOver = popover;
        }
        else
        {
            [[AppController sharedController].viewController presentModalViewController:[VideoPicker sharedPicker].controller animated:YES];
        }
        return true;
    }
    return false;
}

bool pickVideoFromCamera(const std::string& saveName)
{
    if([AppController sharedController] != NULL)
    {
        [[VideoPicker sharedPicker] initController];
        [VideoPicker sharedPicker].saveName = [NSString stringWithFormat:@"%s", saveName.c_str()];
        [[VideoPicker sharedPicker] setSourceType:UIImagePickerControllerSourceTypeCamera];
        [VideoPicker sharedPicker].useCamera = true;
        [[AppController sharedController].viewController presentModalViewController:[VideoPicker sharedPicker].controller animated:YES];
        return true;
    }
    return false;
}

/** Will get all videos from both the camera roll (equivalent to Photos app) and iPod Library (equivalent to Videos app)
 */
void getAllVideos()
{
    //Get videos from camera roll (recorded via device or synchronized via Photos)
    ALAssetsLibrary *assetLibrary = [[ALAssetsLibrary alloc] init];
    
    [assetLibrary enumerateGroupsWithTypes:ALAssetsGroupAll
                                usingBlock:^(ALAssetsGroup *group, BOOL *stop)
     {
         if (group != NULL)
         {
             [group setAssetsFilter:[ALAssetsFilter allVideos]];
             [group enumerateAssetsUsingBlock:^(ALAsset *asset, NSUInteger index, BOOL *stop)
              {
                  if (asset != NULL)
                  {
                      ALAssetRepresentation *representation = [asset defaultRepresentation];
                      const char* path = [[[representation url] absoluteString] UTF8String];
                      notifyVideoFound(path);
                      
                      NSString *fileName = [representation filename];
                      notifyVideoName(path, [fileName UTF8String]);
                      
                      AVURLAsset *sourceAsset = [AVURLAsset URLAssetWithURL:[representation url] options:nil];
                      CMTime duration = sourceAsset.duration;
                      notifyVideoDurationAvailable(path, CMTimeGetSeconds(duration));
                  }
              }];
         }
         else
         {
             notifyGetAllVideosFinished();
         }
     }
                              failureBlock:^(NSError *error)
     {
#warning TODO : warn the user if the error is that he refused the app access to photos
         NSLog(@"error enumerating AssetLibrary groups %@\n", error);
     }
     ];
    
    //Get videos from Video app (synchronized by Video)
    MPMediaQuery *query = [[[MPMediaQuery alloc] init] autorelease];
    [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:[NSNumber numberWithInteger:MPMediaTypeAnyVideo] forProperty:MPMediaItemPropertyMediaType]];
    NSArray *items = [query items];
    for (MPMediaItem* item in items)
    {
        NSString* title = [item valueForProperty:MPMediaItemPropertyTitle];
        NSURL* url = [item valueForProperty:MPMediaItemPropertyAssetURL];
        NSNumber* duration = [item valueForProperty:MPMediaItemPropertyPlaybackDuration];
        if(url != nil)
        {
            std::string path = [[url absoluteString] UTF8String];
            NSLog(@"Sending video with path: %s, title: %s, duration : %f", path.c_str(), [title UTF8String], [duration floatValue]);
            notifyVideoFound(path.c_str());
            notifyVideoName(path.c_str(), [title UTF8String]);
            notifyVideoDurationAvailable(path.c_str(), [duration floatValue]);
        }
    }
}

@implementation VideoPicker

@synthesize controller;
@synthesize popOver;
@synthesize saveName;
@synthesize useCamera;

static VideoPicker* _sharedPicker = nil;

+ (VideoPicker*) sharedPicker
{
	@synchronized([VideoPicker class])
	{
		if (!_sharedPicker)
			[[self alloc] init];
		
		return _sharedPicker;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([VideoPicker class])
	{
		NSAssert(_sharedPicker == nil, @"Attempted to allocate a second instance of VideoPicker singleton.");
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
    controller.mediaTypes =[[NSArray alloc] initWithObjects: (NSString *) kUTTypeMovie, nil];
    controller.delegate = self;
    controller.allowsEditing = NO;
}

// For responding to the user accepting a newly-captured picture or movie
- (void) imagePickerController:(UIImagePickerController*) picker didFinishPickingMediaWithInfo:(NSDictionary*) info
{
    NSString *mediaType = [info objectForKey: UIImagePickerControllerMediaType];
    
    if (CFStringCompare ((CFStringRef) mediaType, kUTTypeMovie, 0) == kCFCompareEqualTo)
    {
        NSURL* videoURL = (NSURL *) [info objectForKey:UIImagePickerControllerMediaURL];
        
        // Copy file into the app
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSError *error;
        NSString* fileName = [NSString stringWithFormat:@"%@/Documents/%@.%@", [NSString stringWithUTF8String:getenv("HOME")], saveName, [videoURL pathExtension]];
        self.saveName = [NSString stringWithFormat:@"%@.%@", saveName, [videoURL pathExtension]];
        if ([fileManager fileExistsAtPath:[videoURL path]] == YES) {
            if(useCamera)
            {// It's useless to keep a tmp video
                [fileManager moveItemAtPath:[videoURL path] toPath:fileName error:&error];
            }
            else
            {
                [fileManager copyItemAtPath:[videoURL path] toPath:fileName error:&error];
            }
        }
        
        notifyVideoPicked([saveName UTF8String]);
        [controller release];
        controller = nil;
    }
    else
    {
        NSLog(@"Problem : picked a media which is not a video");
    }
    [self imagePickerControllerDidCancel:picker];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    // Dismiss the image selection and close the program
    [picker dismissModalViewControllerAnimated:YES];
    if(popOver)
    {
        [popOver dismissPopoverAnimated:YES];
    }
    notifyVideoPickCancelled();
}

@end
