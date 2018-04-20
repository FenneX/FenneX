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

#import "VideoRecorder.h"
#import "AppController.h"
#import "RootViewController.h"
#import <AssetsLibrary/AssetsLibrary.h>
#include "VideoPickerWrapper.h"
#import "platform/ios/CCEAGLView-ios.h"

#define CAPTURE_FRAMES_PER_SECOND		60


//In iOS8, behavior of VideoRecorder was changed. It used to need being rotated for iOS 7 and older, but now it doesn't anymore
#define IS_IOS8_OR_NEWER ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)

@implementation VideoRecorder (Private)

- (void) orientationChanged:(NSNotification*)data
{
	UIInterfaceOrientation orientation = ((UIViewController*)[AppController sharedController].viewController).interfaceOrientation;
	if(UIInterfaceOrientationIsLandscape(orientation) && orientation != currentOrientation)
    {
	if(IS_IOS8_OR_NEWER)
{
        CGAffineTransform rotateTransform = CGAffineTransformMakeRotation(orientation == UIInterfaceOrientationLandscapeLeft ? M_PI_2 : -M_PI_2);
        [self.previewLayer setAffineTransform:rotateTransform];
}
		currentOrientation = orientation;
        [self updateFrame];
	}
}

- (void) updateFrame
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
if(IS_IOS8_OR_NEWER)
{
    [self.previewLayer setPosition:CGPointMake(_position.x, bounds.size.height - _position.y)];
}
else
{
    [self.previewLayer setPosition:CGPointMake((currentOrientation != UIInterfaceOrientationLandscapeLeft ? _position.y : bounds.size.width - _position.y),
                                               (currentOrientation == UIInterfaceOrientationLandscapeLeft ? _position.x : bounds.size.height - _position.x))];
}
}

@end

@implementation VideoRecorder

@synthesize isRecording;
@synthesize previewLayer;

static VideoRecorder* _sharedRecorder = nil;

+ (VideoRecorder*) sharedRecorder
{
	@synchronized([VideoRecorder class])
	{
		if (!_sharedRecorder)
			[[self alloc] init];
		
		return _sharedRecorder;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([VideoRecorder class])
	{
		NSAssert(_sharedRecorder == nil, @"Attempted to allocate a second instance of VideoRecorder singleton.");
		_sharedRecorder = [super alloc];
		return _sharedRecorder;
	}
	
	return nil;
}

- (id) init
{
	self = [super init];
	if (self != nil)
	{//---------------------------------
        //----- SETUP CAPTURE SESSION -----
        //---------------------------------
        NSLog(@"Setting up capture session");
        CaptureSession = [[AVCaptureSession alloc] init];
        
        //----- ADD INPUTS -----
        NSLog(@"Adding video input");
        
        //ADD VIDEO INPUT
        AVCaptureDevice *VideoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
        if (VideoDevice)
        {
            NSError *error;
            VideoInputDevice = [AVCaptureDeviceInput deviceInputWithDevice:VideoDevice error:&error];
            if (!error)
            {
                if ([CaptureSession canAddInput:VideoInputDevice])
                    [CaptureSession addInput:VideoInputDevice];
                else
                    NSLog(@"Couldn't add video input");
            }
            else
            {
                NSLog(@"Couldn't create video input");
            }
        }
        else
        {
            NSLog(@"Couldn't create video capture device");
        }
        
        //ADD AUDIO INPUT
        NSLog(@"Adding audio input");
        AVCaptureDevice *audioCaptureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
        NSError *error = nil;
        AVCaptureDeviceInput *audioInput = [AVCaptureDeviceInput deviceInputWithDevice:audioCaptureDevice error:&error];
        if (audioInput)
        {
            [CaptureSession addInput:audioInput];
        }
        else
        {
            NSLog(@"Problem initiating audio input : %@", [error localizedDescription]);
        }
        
        
        //----- ADD OUTPUTS -----
        
        //ADD VIDEO PREVIEW LAYER
        NSLog(@"Adding video preview layer");
        [self setPreviewLayer:[[[AVCaptureVideoPreviewLayer alloc] initWithSession:CaptureSession] autorelease]];
        
        [self.previewLayer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
        
        
        //ADD MOVIE FILE OUTPUT
        NSLog(@"Adding movie file output");
        MovieFileOutput = [[AVCaptureMovieFileOutput alloc] init];
        
        Float64 TotalSeconds = 60;			//Total seconds
        int32_t preferredTimeScale = 30;	//Frames per second
        CMTime maxDuration = CMTimeMakeWithSeconds(TotalSeconds, preferredTimeScale);	//<<SET MAX DURATION
        MovieFileOutput.maxRecordedDuration = maxDuration;
        
        MovieFileOutput.minFreeDiskSpaceLimit = 1024 * 1024;						//<<SET MIN FREE SPACE IN BYTES FOR RECORDING TO CONTINUE ON A VOLUME
        
        if ([CaptureSession canAddOutput:MovieFileOutput])
            [CaptureSession addOutput:MovieFileOutput];
        
        //SET THE CONNECTION PROPERTIES (output properties)
        [self cameraSetOutputProperties:VideoDevice];			//(We call a method as it also has to be done after changing camera)
        
        
        
        //----- SET THE IMAGE QUALITY / RESOLUTION -----
        //Options:
        //	AVCaptureSessionPresetHigh - Highest recording quality (varies per device)
        //	AVCaptureSessionPresetMedium - Suitable for WiFi sharing (actual values may change)
        //	AVCaptureSessionPresetLow - Suitable for 3G sharing (actual values may change)
        //	AVCaptureSessionPreset640x480 - 640x480 VGA (check its supported before setting it)
        //	AVCaptureSessionPreset1280x720 - 1280x720 720p HD (check its supported before setting it)
        //	AVCaptureSessionPresetPhoto - Full photo resolution (not supported for video output)
        NSLog(@"Setting image quality");
        [CaptureSession setSessionPreset:AVCaptureSessionPresetMedium];
        if ([CaptureSession canSetSessionPreset:AVCaptureSessionPreset640x480])		//Check size based configs are supported before setting them
            [CaptureSession setSessionPreset:AVCaptureSessionPreset640x480];
        
        
        
        //----- DISPLAY THE PREVIEW LAYER -----
        //Display it full screen under out view controller existing controls
        NSLog(@"Display the preview layer");
        UIView* mainView = [AppController sharedController].viewController.view.superview;
        CGRect layerRect = [[mainView layer] bounds];
        [self.previewLayer setBounds:layerRect];
        [self.previewLayer setPosition:CGPointMake(CGRectGetMidX(layerRect),
                                              CGRectGetMidY(layerRect))];
        self.previewLayer.videoGravity = AVLayerVideoGravityResizeAspect;
        //[[[self view] layer] addSublayer:[[self CaptureManager] previewLayer]];
        //We use this instead so it goes on a layer behind our UI controls (avoids us having to manually bring each control to the front):
        cameraView = [[[UIView alloc] init] autorelease];
        
        [mainView addSubview:cameraView];
        [mainView sendSubviewToBack:cameraView];
        
        
        //----- START THE CAPTURE SESSION RUNNING -----
        [CaptureSession startRunning];
        
        isRecording = NO;
		currentOrientation = UIInterfaceOrientationPortrait;
        [self orientationChanged:nil];
        [self updateFrame];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
    }
    return self;
}



//********** CAMERA SET OUTPUT PROPERTIES **********
- (void) cameraSetOutputProperties:(AVCaptureDevice*)device
{
	//SET THE CONNECTION PROPERTIES (output properties)
	AVCaptureConnection *CaptureConnection = [MovieFileOutput connectionWithMediaType:AVMediaTypeVideo];
	
	//Set landscape (if required)
	if ([CaptureConnection isVideoOrientationSupported])
	{
		AVCaptureVideoOrientation orientation = AVCaptureVideoOrientationLandscapeRight;		//<<<<<SET VIDEO ORIENTATION IF LANDSCAPE
		[CaptureConnection setVideoOrientation:orientation];
	}
    
	//Set frame rate (if required)
    NSLog(@"Initial min and max frame duration");
	CMTimeShow(device.activeVideoMinFrameDuration);
	CMTimeShow(device.activeVideoMaxFrameDuration);
    
    AVCaptureDeviceFormat *bestFormat = nil;
    AVFrameRateRange *bestFrameRateRange = nil;
    for ( AVCaptureDeviceFormat *format in [device formats] ) {
        for ( AVFrameRateRange *range in format.videoSupportedFrameRateRanges ) {
            if ( range.maxFrameRate > bestFrameRateRange.maxFrameRate ) {
                bestFormat = format;
                bestFrameRateRange = range;
            }
        }
    }
    if ( bestFormat ) {
        if ( [device lockForConfiguration:NULL] == YES ) {
            device.activeFormat = bestFormat;
            device.activeVideoMinFrameDuration = bestFrameRateRange.minFrameDuration;
            device.activeVideoMaxFrameDuration = bestFrameRateRange.minFrameDuration;
            [device unlockForConfiguration];
        }
    }
    NSLog(@"Device set to best frame duration: (min/max)");
    CMTimeShow(device.activeVideoMinFrameDuration);
    CMTimeShow(device.activeVideoMaxFrameDuration);
}

//********** GET CAMERA IN SPECIFIED POSITION IF IT EXISTS **********
- (AVCaptureDevice *) cameraWithPosition:(AVCaptureDevicePosition)position
{
	NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
	for (AVCaptureDevice *device in devices)
	{
		if ([device position] == position)
		{
			return device;
		}
	}
	return nil;
}

- (void) setPreviewPosition:(CGPoint)position size:(CGSize)size
{
    _position = position;
    cocos2d::GLView *glview = cocos2d::Director::getInstance()->getOpenGLView();
    CCEAGLView *eaglview = (CCEAGLView*) glview->getEAGLView();
    float scale = [eaglview contentScaleFactor];
    _position = CGPointMake(_position.x / scale, _position.y / scale);
    [self.previewLayer setBounds:CGRectMake(0, 0, size.height / scale, size.width / scale)];
    [self updateFrame];
}

- (void) startPreview
{
    if(![[cameraView layer].sublayers containsObject:self.previewLayer])
    {
        [[cameraView layer] addSublayer:self.previewLayer];
    }
}

- (void) stopPreview
{
    if([[cameraView layer].sublayers containsObject:self.previewLayer])
    {
        [self.previewLayer removeFromSuperlayer];
    }
}

//********** START STOP RECORDING BUTTON **********
- (void) startRecord
{
	if (!isRecording)
	{
		NSLog(@"START RECORDING");
		isRecording = YES;
		[self startPreview];
        
		//Create temporary URL to record to
		NSString *outputPath = [[NSString alloc] initWithFormat:@"%@%@", NSTemporaryDirectory(), @"output.mov"];
		NSURL *outputURL = [[NSURL alloc] initFileURLWithPath:outputPath];
		NSFileManager *fileManager = [NSFileManager defaultManager];
		if ([fileManager fileExistsAtPath:outputPath])
		{
			NSError *error;
			if ([fileManager removeItemAtPath:outputPath error:&error] == NO)
			{
                NSLog(@"Error when trying to remove old temp video : %@", [error localizedDescription]);
			}
		}
		[outputPath release];
		//Start recording
		[MovieFileOutput startRecordingToOutputFileURL:outputURL recordingDelegate:self];
		[outputURL release];
        shouldDeleteNextVideo = NO;
	}
    else
    {
        NSLog(@"Can't start video recording : already started");
    }
}

- (void) stopRecord
{
    if(isRecording)
	{
		NSLog(@"STOP RECORDING");
		isRecording = NO;
        [self stopPreview];
        
		[MovieFileOutput stopRecording];
	}
    else
    {
        NSLog(@"Can't stop video recording : not started");
    }
}

- (BOOL) cancelRecording:(BOOL)notify
{
    BOOL stopped = false;
    if([[cameraView layer].sublayers containsObject:self.previewLayer])
    {
        [self.previewLayer removeFromSuperlayer];
        stopped = true;
    }
    if(isRecording)
    {
		isRecording = NO;
        shouldDeleteNextVideo = YES;
		[MovieFileOutput stopRecording];
        stopped = true;
    }
    if(stopped && notify)
    {
        notifyRecordingCancelled();
    }
    return stopped;
}

- (BOOL) canToggleCamera
{
    return [[AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo] count] > 1;
}


//********** CAMERA TOGGLE **********
- (void) toggleCamera
{
	if ([self canToggleCamera])		//Only do if device has multiple cameras
	{
		NSLog(@"Toggle camera");
		NSError *error;
		//AVCaptureDeviceInput *videoInput = [self videoInput];
		AVCaptureDeviceInput *NewVideoInput = nil;
		AVCaptureDevicePosition position = [[VideoInputDevice device] position];
        
        AVCaptureDevice *VideoDevice = nil;
		if (position == AVCaptureDevicePositionBack)
		{
            VideoDevice = [self cameraWithPosition:AVCaptureDevicePositionFront];
			NewVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:VideoDevice error:&error];
		}
		else if (position == AVCaptureDevicePositionFront)
        {
            VideoDevice = [self cameraWithPosition:AVCaptureDevicePositionFront];
			NewVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:VideoDevice error:&error];
		}
        
		if (NewVideoInput != nil)
		{
			[CaptureSession beginConfiguration];		//We can now change the inputs and output configuration.  Use commitConfiguration to end
			[CaptureSession removeInput:VideoInputDevice];
			if ([CaptureSession canAddInput:NewVideoInput])
			{
				[CaptureSession addInput:NewVideoInput];
				VideoInputDevice = NewVideoInput;
			}
			else
			{
				[CaptureSession addInput:VideoInputDevice];
			}
			
			//Set the connection properties again
            [self cameraSetOutputProperties:VideoDevice];
			
			
			[CaptureSession commitConfiguration];
			[NewVideoInput release];
		}
	}
}





//********** DID FINISH RECORDING TO OUTPUT FILE AT URL **********
- (void)captureOutput:(AVCaptureFileOutput *)captureOutput
didFinishRecordingToOutputFileAtURL:(NSURL *)outputFileURL
	  fromConnections:(NSArray *)connections
				error:(NSError *)error
{
    
	NSLog(@"didFinishRecordingToOutputFileAtURL - enter");
	
    BOOL RecordedSuccessfully = YES;
    if ([error code] != noErr)
	{
        // A problem occurred: Find out if the recording was successful.
        id value = [[error userInfo] objectForKey:AVErrorRecordingSuccessfullyFinishedKey];
        if (value)
		{
            RecordedSuccessfully = [value boolValue];
        }
    }
    if(shouldDeleteNextVideo)
    {
        error = NULL;
        if(![[NSFileManager defaultManager] removeItemAtURL:outputFileURL error: &error])
        {
            NSLog(@"Couldn't delete video at path %@ after cancel", [outputFileURL absoluteString]);
        }
        else
        {
            NSLog(@"File %@ deleted as the video recording was canceled", [outputFileURL absoluteString]);
        }
    }
	else if (RecordedSuccessfully)
	{
		//----- RECORDED SUCESSFULLY -----
        NSLog(@"didFinishRecordingToOutputFileAtURL - success");
		ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
		if ([library videoAtPathIsCompatibleWithSavedPhotosAlbum:outputFileURL])
		{
			[library writeVideoAtPathToSavedPhotosAlbum:outputFileURL
										completionBlock:^(NSURL *assetURL, NSError *error)
             {
                 if (error)
                 {
                     NSLog(@"Error when trying to write video in photo album : %@", [error localizedDescription]);
                 }
                 else if(assetURL == NULL)
                 {
                     NSLog(@"Error : assertURL NULL, video too short ?");
                 }
                 else
                 {
                     /*
                     [library assetForURL:assetURL
                              resultBlock:^(ALAsset* alAsset) {
                                  // do whatever you want with alAsset
                              }
                             failureBlock:^(NSError* error) {
                                 // do whatever you want with alAsset
                             }];*/
                     NSLog(@"Assets url : %@, path : %@", assetURL, [assetURL absoluteString]);
                     ALAssetsLibraryAssetForURLResultBlock resultblock = ^(ALAsset *myasset)
                     {
                         ALAssetRepresentation *representation = [myasset defaultRepresentation];
                         NSString *fileName = [representation filename];
                         notifyVideoName([[assetURL absoluteString] UTF8String], [fileName UTF8String]);
                     };
                     //Warning : the MediaURL is the path to the readable file (which may have been compressed)
                     //while ReferenceURL is the original AssetsLibrary path (can't be read directly)
                     ALAssetsLibrary* assetslibrary = [[[ALAssetsLibrary alloc] init] autorelease];
                     [assetslibrary assetForURL:assetURL
                                    resultBlock:resultblock
                                   failureBlock:nil];
                     notifyVideoPicked([[assetURL absoluteString] UTF8String], FileLocation::Absolute);
                 }
             }];
		}
        
		[library release];
		
	}
}

//********** DEALLOC **********
- (void)dealloc
{
	[CaptureSession release];
	[MovieFileOutput release];
	[VideoInputDevice release];
    
	[super dealloc];
}

@end
