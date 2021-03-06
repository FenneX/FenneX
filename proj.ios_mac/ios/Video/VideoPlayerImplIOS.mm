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

#import "VideoPlayerImplIOS.h"
#import "AppController.h"
#include "FenneX.h"
#include "VideoPlayer.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <AVFoundation/AVFoundation.h>
#import "UIImage+Resize.h"

USING_NS_FENNEX;

@implementation VideoPlayerImplIOS (Private)

- (void) orientationChanged:(NSNotification*)data
{
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    if(UIInterfaceOrientationIsLandscape(orientation) && orientation != currentOrientation)
    {
        CGAffineTransform transform = CGAffineTransformIdentity;
        if(isFullScreen)
        {
            CGRect bounds = [[UIScreen mainScreen] bounds];
            float scale = 0;
            if(_size.height / bounds.size.height > _size.width / bounds.size.width)
            {
                scale = bounds.size.height / _size.height;
            }
            else
            {
                scale = bounds.size.width / _size.width;
            }
            transform = CGAffineTransformScale(transform, scale, scale);
        }
        //If there is no data, it's called from self at startup : don't animate the transition
        [UIView transitionWithView:playerController.view duration:data == nil ? 0 : 0.5 options:UIViewAnimationOptionTransitionNone
                        animations:^{playerController.view.transform = transform;}
                        completion:nullptr];
        currentOrientation = orientation;
        [self updateFrame];
    }
}

- (void) updateFrame
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    if(isFullScreen)
    {
        [playerController.view setCenter:CGPointMake(bounds.size.width / 2, bounds.size.height/2)];
    }
    else
    {
        //Y-axis is inverted
        [playerController.view setCenter:CGPointMake(_position.x, bounds.size.height - _position.y)];
    }
}

- (void)playerItemDidReachEnd:(NSNotification *)notification {
    [playerController.player seekToTime:kCMTimeZero];
    if(!_loop)
    {
        [playerController.player pause];
    }
    notifyVideoEnded([path UTF8String]);
}

- (void)playerItemFailedToPlay:(NSNotification *)notification {
    notifyVideoError([path UTF8String]);
}

@end

@implementation VideoPlayerImplIOS

@synthesize hideOnPause;


- (id) initWithPlayFile:(NSString*)file position:(CGPoint)position size:(CGSize)size front:(BOOL)front loop:(BOOL)loop
{
    self = [super init];
    if(self != nil)
    {
        hideOnPause = YES;
        _position = position;
        _size = size;
        isFullScreen = NO;
        path = file;
        [path retain];
        currentOrientation = UIInterfaceOrientationPortrait;
        desiredPlaybackRate = 1;
        NSURL* movieURL = [VideoPlayerImplIOS URLFromPath:file];
        NSLog(@"Movie full path : %@", [movieURL absoluteString]);
        playerController = [[AVPlayerViewController alloc] init];
        playerController.player = [AVPlayer playerWithURL:movieURL];
        
        playerController.player.actionAtItemEnd = AVPlayerActionAtItemEndNone;
        playerController.showsPlaybackControls = NO;
        _loop = loop;
        
        playerController.view.userInteractionEnabled = NO;
        
        [playerController.view setBounds:CGRectMake(0, 0, _size.width, _size.height)];
        UIViewController* rootVC = (UIViewController*)[AppController sharedController].viewController;
        if(front)
        {
            [rootVC.view.superview addSubview:playerController.view];
        }
        else
        {
            [rootVC.view.superview insertSubview:playerController.view atIndex:0];
        }
        [self orientationChanged:nil];
        [self updateFrame];
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(durationAvailable:) name:@"MPMovieDurationAvailableNotification" object:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playerItemDidReachEnd:) name:AVPlayerItemDidPlayToEndTimeNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(playerItemFailedToPlay:) name:AVPlayerItemFailedToPlayToEndTimeNotification object:nil];
        playerController.view.hidden = YES;
    }
    return self;
}

- (void) setPlayerPosition:(CGPoint)position size:(CGSize)size animated:(BOOL)animated
{
    _position = position;
    if(!animated)
    {
        [playerController.view setBounds:CGRectMake(0, 0, _size.width, _size.height)];
        
        CGRect bounds = [[UIScreen mainScreen] bounds];
        CGPoint newCenter = CGPointMake(_position.x, bounds.size.height - _position.y);
        playerController.view.center = newCenter;
    }
    else
    {
        CGSize originalSize = playerController.player.currentItem.tracks.firstObject.assetTrack.naturalSize;
        //Force the size to have the same ratio as the original size, otherwise it causes problems
        if(originalSize.width > 0 && originalSize.height > 0)
        {
            if(originalSize.width / originalSize.height > size.width / size.height)
            {
                size.height = size.width * originalSize.height / originalSize.width;
            }
            else
            {
                size.width = size.height * originalSize.width / originalSize.height;
            }
        }
        CGRect bounds = [[UIScreen mainScreen] bounds];
        
        CGPoint newCenter = CGPointMake(_position.x, bounds.size.height - _position.y);;
        float scale = 0;
        if(size.height / _size.height > size.width / _size.width)
        {
            scale = size.height / _size.height;
        }
        else
        {
            scale = size.width / _size.width;
        }
        
        [UIView animateWithDuration: 0.5
                              delay: 0
                            options: (UIViewAnimationOptionCurveLinear | UIViewAnimationOptionAllowUserInteraction)
                         animations:^{playerController.view.center = newCenter ; playerController.view.transform = CGAffineTransformScale(playerController.view.transform, scale, scale);}
                         completion:^(BOOL finished) { }
         ];
        
        _size = size;
    }
}

- (void) play
{
    [playerController.player play];
    playerController.view.hidden = NO;
    playerController.player.rate = desiredPlaybackRate;
}

- (void) pause
{
    [playerController.player pause];
    if(hideOnPause)
    {
        playerController.view.hidden = YES;
    }
}

- (void) stop
{
    [playerController.player pause];
    [playerController.player seekToTime:kCMTimeZero];
    playerController.view.hidden = YES;
}

- (float) playbackRate
{
    return playerController.player.rate;
}

- (void) setPlaybackRate:(float)rate
{
    if(playerController.player.rate != 0)
    {
        playerController.player.rate = rate;
    }
    desiredPlaybackRate = rate;
}

- (BOOL) fullscreen
{
    return isFullScreen;
}

- (float) duration
{
    return float(CMTimeGetSeconds(playerController.player.currentItem.duration));
}

- (float) position
{
    return float(CMTimeGetSeconds(playerController.player.currentTime));
}

- (void) setPosition:(float)position
{
    [playerController.player seekToTime:CMTimeMake(position, 1)];
}

- (void) setFullscreen:(BOOL)fullscreen animated:(BOOL)animated
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    if(!animated)
    {
        if(fullscreen)
        {
            //For whatever reason, iOS invert height and width ...
            [playerController.view setBounds:CGRectMake(0, 0, bounds.size.height, bounds.size.width)];
        }
        else
        {
            
            [playerController.view setBounds:CGRectMake(0, 0, _size.width, _size.height)];
        }
        [self updateFrame];
    }
    else
    {
        CGSize originalSize = playerController.player.currentItem.tracks.firstObject.assetTrack.naturalSize;
        //Force the size to have the same ratio as the original size, otherwise it causes problems
        if(originalSize.width > 0 && originalSize.height > 0)
        {
            if(originalSize.width / originalSize.height > _size.width / _size.height)
            {
                _size.height = _size.width * originalSize.height / originalSize.width;
            }
            else
            {
                _size.width = _size.height * originalSize.width / originalSize.height;
            }
        }
        
        CGPoint newCenter = fullscreen ? CGPointMake(bounds.size.width / 2, bounds.size.height/2) :
            CGPointMake(_position.x, bounds.size.height - _position.y);;
        float scale = 0;
        if(_size.height / bounds.size.height > _size.width / bounds.size.width)
        {
            scale = fullscreen ? bounds.size.height / _size.height : _size.height / bounds.size.height;
        }
        else
        {
            scale = fullscreen ? bounds.size.width / _size.width : _size.width / bounds.size.width;
        }
        
        [UIView animateWithDuration: 0.5
                              delay: 0
                            options: (UIViewAnimationOptionCurveLinear | UIViewAnimationOptionAllowUserInteraction)
                         animations:^{playerController.view.center = newCenter ; playerController.view.transform = CGAffineTransformScale(playerController.view.transform, scale, scale);}
                         completion:^(BOOL finished) { }
         ];
        
        
    }
    isFullScreen = fullscreen;
    
    //This line prevents touches from going through, which is a big problem. It also put the Video in front
    //[player setFullscreen:fullscreen animated:animated];
}

- (void) setMuted:(BOOL)muted
{
    playerController.player.muted = muted;
}

- (void) durationAvailable:(NSNotification*)notif
{
    notifyVideoDurationAvailable([path UTF8String], self.duration);
}

+ (NSURL*) URLFromPath:(NSString*)file
{
    NSString* extension = [file pathExtension];
    NSString* withoutExtension = file;
    if(extension.length == 0)
    {
        extension = @"mov";
    }
    else
    {
        withoutExtension = [file stringByDeletingPathExtension];
    }
    NSString* moviePath = [[NSBundle mainBundle] pathForResource:withoutExtension ofType:extension];
    if(moviePath == nullptr)
    {
        moviePath = file;
    }
    return [moviePath hasPrefix:@"assets-library://"] || [moviePath hasPrefix:@"ipod-library://"] ? [NSURL URLWithString:moviePath] :  [NSURL fileURLWithPath:moviePath];
}

+ (BOOL) getScreenshot:(NSString*)path screenshotName:(NSString*)screenshotName
{
    //NSString* screenshotPath = [path stringByReplacingOccurrencesOfString:[path lastPathComponent] withString:screenshotFileName];
    UIImage *screenshot = nullptr;
    //Only generate it if it doesn't exist
    //This method is badly named, it will check for ANY file, not just videos
    if(![VideoPlayerImplIOS videoExists:screenshotName])
    {
        //Try to get the screenshot from Photo app first
        AVURLAsset *asset = [[[AVURLAsset alloc] initWithURL:[VideoPlayerImplIOS URLFromPath:path] options:nil] autorelease];
        AVAssetImageGenerator *generateImg = [[[AVAssetImageGenerator alloc] initWithAsset:asset] autorelease];
        NSError *error = nullptr;
        CMTime time = CMTimeMake(1, 1);
        CGImageRef refImg = [generateImg copyCGImageAtTime:time actualTime:nullptr error:&error];
        if(error == nullptr)
        {
            screenshot = [[[UIImage alloc] initWithCGImage:refImg] autorelease];
            //Fix the screenshot orientation by rotating it (on iOS, the video can be rotated and there is a metadata that indicate that)
            AVAssetTrack *videoAssetTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
            CGAffineTransform videoTransform = videoAssetTrack != nil ? videoAssetTrack.preferredTransform : CGAffineTransformIdentity;
            screenshot = [screenshot imageRotatedWithTransform:videoTransform];
        }
        //The asset is not available on Photos app, try to get it from Videos app
        else
        {
            MPMediaPropertyPredicate *predicateMovie = [MPMediaPropertyPredicate predicateWithValue:[NSNumber numberWithInteger:MPMediaTypeAnyVideo] forProperty:MPMediaItemPropertyMediaType];
            MPMediaQuery *query = [[[MPMediaQuery alloc] init] autorelease];
            [query addFilterPredicate:predicateMovie];
            NSArray *items = [query items];
            MPMediaItem* movie = nil;
            for (MPMediaItem* item in items)
            {
                NSURL* url = [item valueForProperty:MPMediaItemPropertyAssetURL];
                if([[url absoluteString] isEqualToString:path])
                {
                    movie = item;
                }
            }
            if(movie != nil)
            {
                MPMediaItemArtwork *itemArtwork = [movie valueForProperty:MPMediaItemPropertyArtwork];
                if(itemArtwork != nil)
                {
                    screenshot = [itemArtwork imageWithSize:[itemArtwork bounds].size];
                }
            }
        }
        
        if(screenshot != nullptr)
        {
            BOOL result = [UIImagePNGRepresentation(screenshot) writeToFile:screenshotName options:NSDataWritingAtomic error:&error];
            NSLog(@"Write result for screenshot %@ : %@", screenshotName, (result ? @"OK" : @"Problem"));
            if(result)
            {
                Director::getInstance()->getTextureCache()->removeTextureForKey([screenshotName UTF8String]);
            }
            else
            {
                NSLog(@"Write error for screenshot description: %@, reason: %@", [error localizedDescription], [error localizedFailureReason]);
            }
            return result;
        }
        else
        {
            return FALSE;
        }
    }
    return YES;
}

+ (CGSize) getVideoSize:(NSString*)path
{
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:[VideoPlayerImplIOS URLFromPath:path] options:nil];
    AVAssetTrack *track = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
    return track.naturalSize;
}

+ (BOOL) videoExists:(NSString*)path
{
    if([path hasPrefix:@"assets-library://"])
    {
        ALAssetsLibrary *library = [[[ALAssetsLibrary alloc] init] autorelease];
        [library assetForURL:[self URLFromPath:path]
                 resultBlock:^(ALAsset *asset){
                     if (asset)
                     {
                         notifyVideoExists([path UTF8String]);
                     }
                     else
                     {
                         notifyVideoRemoved([path UTF8String]);
                     }
                 }
                failureBlock:nil];
        return YES;
    }
    else if([path hasPrefix:@"ipod-library://"])
    {
        MPMediaQuery *query = [[[MPMediaQuery alloc] init] autorelease];
        [query addFilterPredicate:[MPMediaPropertyPredicate predicateWithValue:[NSNumber numberWithInteger:MPMediaTypeAnyVideo] forProperty:MPMediaItemPropertyMediaType]];
        NSArray *items = [query items];
        BOOL found = NO;
        for (MPMediaItem* item in items)
        {
            NSURL* url = [item valueForProperty:MPMediaItemPropertyAssetURL];
            if([path isEqualToString:[url absoluteString]])
            {
                found = YES;
            }
        }
        if (found)
        {
            notifyVideoExists([path UTF8String]);
        }
        else
        {
            notifyVideoRemoved([path UTF8String]);
        }
        return found;
    }
    //DO NOT use NSFileManager fileExistsAtPath since it always returns NO for trimmed videos in temporary folder
    //Instead, try to open the file, as suggested by Apple documentation
    NSData* data = [NSData dataWithContentsOfURL:[self URLFromPath:path]];
    //Use an arbitrary minimum length to filter out empty/malformed files
    return data != nil && data.length > 100;
}

- (void)dealloc
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    NSLog(@"Video player impl dealloc");
    
    [playerController.player pause];
    [playerController.view removeFromSuperview];
    [playerController release];
    [path release];
    [super dealloc];
}

@end
