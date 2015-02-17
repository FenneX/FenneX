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

#import "AudioPicker.h"
#import "AppController.h"
#import "NSFileManager+ApplicationSupport.h"

#include "AppMacros.h"
#include "FenneX.h"
#include "AudioPickerWrapper.h"

#include "TargetConditionals.h"

USING_NS_CC;
USING_NS_FENNEX;

bool pickSound(const char* promptText, const char* saveName, const char* identifier)
{
#if !(TARGET_IPHONE_SIMULATOR)
    [[AudioPicker sharedPicker] initController];
    [AudioPicker sharedPicker].controller.prompt = [NSString stringWithCString:promptText encoding:NSUTF8StringEncoding];
    [AudioPicker sharedPicker].saveName = [NSString stringWithFormat:@"%s.m4a", saveName];
    [AudioPicker sharedPicker].identifier = [NSString stringWithFormat:@"%s", identifier];
    NSLog(@"Picking sound %s", saveName);
    if([AppController sharedController] != NULL)
    {
        [[AppController sharedController].viewController presentModalViewController:[AudioPicker sharedPicker].controller animated:YES];
        return true;
    }
    return false;
#else
    NSLog(@"Can't pick sound %s, simulator doesn't support it", saveName);
    return false;
#endif
}

bool isAudioPickerExporting()
{
    return [AudioPicker sharedPicker].isExporting;
}

const char* audioPickerCurrentExport()
{
    return isAudioPickerExporting() ? [[AudioPicker sharedPicker].saveName UTF8String] : NULL;
}

void stopAudioPickerExport()
{
    [[AudioPicker sharedPicker] cancelExport];
}

@implementation AudioPicker
@synthesize controller;
@synthesize saveName;
@synthesize identifier;

- (BOOL) isExporting
{
    return exporter != NULL;
}

static AudioPicker* _sharedPicker = nil;

+ (AudioPicker*) sharedPicker
{
	@synchronized([AudioPicker class])
	{
		if (!_sharedPicker)
			[[self alloc] init];
		
		return _sharedPicker;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([AudioPicker class])
	{
		NSAssert(_sharedPicker == nil, @"Attempted to allocate a second instance of AudioPicker singleton.");
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
        exporter = NULL;
    }
    return self;
}

- (void) initController
{
    [controller release];
    controller = [[MPMediaPickerController alloc] init];
    controller.delegate = self;
    controller.allowsPickingMultipleItems = NO;
}

- (void) mediaPicker:(MPMediaPickerController*)mediaPicker didPickMediaItems:(MPMediaItemCollection*)collection
{
    MPMediaItem* item = [collection.items objectAtIndex:0];
    NSLog(@"selected title : %@", [item valueForProperty:MPMediaItemPropertyTitle]);
    [self saveSong:item toFile:saveName];
    notifySoundPicked([saveName UTF8String], [identifier UTF8String]);
    [self mediaPickerDidCancel:mediaPicker];
}

- (void) mediaPickerDidCancel:(MPMediaPickerController *)mediaPicker
{
    [mediaPicker dismissModalViewControllerAnimated: YES];
}

- (void) saveSong:(MPMediaItem*)song toFile:(NSString*)targetFile
{    
    AVURLAsset *songAsset = [AVURLAsset URLAssetWithURL: [song valueForProperty: MPMediaItemPropertyAssetURL] options:nil];
    NSLog(@"compatible presets : %@", [AVAssetExportSession exportPresetsCompatibleWithAsset:songAsset]);
    exporter = [[AVAssetExportSession alloc] initWithAsset: songAsset
                                                                      presetName: AVAssetExportPresetAppleM4A];
    NSLog (@"created exporter. supportedFileTypes: %@", exporter.supportedFileTypes);
    if(![exporter.supportedFileTypes containsObject:AVFileTypeAppleM4A])
    {
        NSLog(@"exporter doesn't support quick time movie, can't export the audio");
        return;
    }
    
    exporter.outputFileType = AVFileTypeAppleM4A;
    NSString *exportFile = [[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:
                            targetFile];
    exporter.outputURL = [NSURL fileURLWithPath:exportFile];
    
#if USE_SYNCHRONOUS_AUDIO_PICKER_EXPORT
    //Synchronous export
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
#endif
    [targetFile retain];
    [identifier retain];
    [exporter exportAsynchronouslyWithCompletionHandler:^{
        
        int exportStatus = exporter.status;
        [exporter release];
        exporter = NULL;
        switch (exportStatus) {
            case AVAssetExportSessionStatusFailed:                
                NSLog (@"AVAssetExportSessionStatusFailed: %@", exporter.error);
                break;
            case AVAssetExportSessionStatusCompleted:             
                NSLog (@"AVAssetExportSessionStatusCompleted");
                notifySoundEncoded([targetFile UTF8String], [identifier UTF8String]);
                break;   
            case AVAssetExportSessionStatusUnknown:
                NSLog (@"AVAssetExportSessionStatusUnknown");
                break;
            case AVAssetExportSessionStatusExporting:
                NSLog (@"AVAssetExportSessionStatusExporting");
                break;                
            case AVAssetExportSessionStatusCancelled:
                NSLog (@"AVAssetExportSessionStatusCancelled");
                break;                
            case AVAssetExportSessionStatusWaiting: 
                NSLog (@"AVAssetExportSessionStatusWaiting");
                break;                
            default: 
                NSLog (@"didn't get export status"); 
                break;
        }
#if USE_SYNCHRONOUS_AUDIO_PICKER_EXPORT
        dispatch_semaphore_signal(semaphore);
#endif        
        [targetFile release];
        [identifier release];
    }];
#if USE_SYNCHRONOUS_AUDIO_PICKER_EXPORT
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    dispatch_release(semaphore);
#endif
}

- (void) cancelExport
{
    [exporter cancelExport];
}

@end
