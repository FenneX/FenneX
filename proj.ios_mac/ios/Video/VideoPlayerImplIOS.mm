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
    //TODO : fix me
	UIInterfaceOrientation orientation = ((UIViewController*)[AppController sharedController].viewController).interfaceOrientation;
	if(UIInterfaceOrientationIsLandscape(orientation) && orientation != currentOrientation)
	{
        CGAffineTransform transform = CGAffineTransformIdentity;
        if(isFullScreen)
        {
            CGRect bounds = [[UIScreen mainScreen] bounds];
            float scale = 0;
            if(_size.width / bounds.size.height > _size.height / bounds.size.width)
            {
                scale = bounds.size.height / _size.width;
            }
            else
            {
                scale = bounds.size.width / _size.height;
            }
            transform = CGAffineTransformScale(transform, scale, scale);
        }
        //If there is no data, it's called from self at startup : don't animate the transition
		[UIView transitionWithView:player.view duration:data == nil ? 0 : 0.5 options:UIViewAnimationOptionTransitionNone
						animations:^{player.view.transform = transform;}
						completion:NULL];
		currentOrientation = orientation;
        [self updateFrame];
	}
}

- (void) updateFrame
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    if(isFullScreen)
    {
        [player.view setCenter:CGPointMake(bounds.size.width / 2, bounds.size.height/2)];
    }
    else
    {
        //Y-axis is inverted
        [player.view setCenter:CGPointMake(_position.x, bounds.size.height - _position.y)];
    }
}

- (void) videoEnded:(NSNotification*)data
{
    int reason = [[[data userInfo] valueForKey:MPMoviePlayerPlaybackDidFinishReasonUserInfoKey] intValue];
    if (reason == MPMovieFinishReasonPlaybackEnded)
    {
        notifyVideoEnded([path UTF8String]);
    }
    else if (reason == MPMovieFinishReasonPlaybackError)
    {
        NSLog(@"Error when trying to play video");
    }
    else if (reason == MPMovieFinishReasonUserExited)
    {
        //can't happen because user can't exit
    }
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
		player = [[MPMoviePlayerController alloc] initWithContentURL:movieURL];
		
		player.shouldAutoplay = NO;
		player.scalingMode = MPMovieScalingModeAspectFit;
		player.fullscreen = false;
        player.repeatMode = loop ? MPMovieRepeatModeOne : MPMovieRepeatModeNone;
		player.controlStyle = MPMovieControlStyleNone;
		player.view.userInteractionEnabled = NO;
        
        [player.view setBounds:CGRectMake(0, 0, _size.width, _size.height)];
        UIViewController* rootVC = (UIViewController*)[AppController sharedController].viewController;
        if(front)
        {
            [rootVC.view.superview addSubview:player.view];
        }
        else
        {
            [rootVC.view.superview insertSubview:player.view atIndex:0];
        }
		[self orientationChanged:nil];
        [self updateFrame];
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(durationAvailable:) name:@"MPMovieDurationAvailableNotification" object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(videoEnded:) name:@"MPMoviePlayerPlaybackDidFinishNotification" object:nil];
        player.view.hidden = YES;
    }
    return self;
}

- (void) play
{
    [player play];
    player.view.hidden = NO;
    player.currentPlaybackRate = desiredPlaybackRate;
}

- (void) pause
{
    [player pause];
    if(hideOnPause)
    {
        player.view.hidden = YES;
    }
}

- (void) stop
{
    [player stop];
    player.view.hidden = YES;
}

- (float) playbackRate
{
    return player.playbackState == MPMoviePlaybackStatePlaying ? player.currentPlaybackRate : desiredPlaybackRate;
}

- (void) setPlaybackRate:(float)rate
{
    BOOL playing = (player.playbackState == MPMoviePlaybackStatePlaying);
    if(playing)
    {
        player.currentPlaybackRate = rate;
    }
    desiredPlaybackRate = rate;
}

- (BOOL) fullscreen
{
    return isFullScreen;
}

- (float) duration
{
    return player.duration;
}

- (float) position
{
    return player.currentPlaybackTime;
}

- (void) setPosition:(float)position
{
    [player setCurrentPlaybackTime:position];
}

- (void) setFullscreen:(BOOL)fullscreen animated:(BOOL)animated
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    if(!animated)
    {
        if(fullscreen)
        {
            //For whatever reason, iOS invert height and width ...
            [player.view setBounds:CGRectMake(0, 0, bounds.size.height, bounds.size.width)];
        }
        else
        {
            
            [player.view setBounds:CGRectMake(0, 0, _size.width, _size.height)];
        }
        [self updateFrame];
    }
    else
    {
        CGSize originalSize = player.naturalSize;
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
                                        CGPointMake((currentOrientation == UIInterfaceOrientationLandscapeRight ? _position.y : bounds.size.width - _position.y),
                                           (currentOrientation == UIInterfaceOrientationLandscapeLeft ? _position.x : bounds.size.height - _position.x));
        float scale = 0;
        if(_size.width / bounds.size.height > _size.height / bounds.size.width)
        {
            scale = fullscreen ? bounds.size.height / _size.width : _size.width / bounds.size.height;
        }
        else
        {
            scale = fullscreen ? bounds.size.width / _size.height : _size.height / bounds.size.width;
        }
        
        [UIView animateWithDuration: 0.5
                              delay: 0
                            options: (UIViewAnimationOptionCurveLinear | UIViewAnimationOptionAllowUserInteraction)
                         animations:^{player.view.center = newCenter ; player.view.transform = CGAffineTransformScale(player.view.transform, scale, scale);}
                         completion:^(BOOL finished) { }
         ];
        

    }
    isFullScreen = fullscreen;
    
    //This line prevents touches from going through, which is a big problem. It also put the Video in front
    //[player setFullscreen:fullscreen animated:animated];
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
    if(moviePath == NULL)
    {
        moviePath = file;
    }
    return [moviePath hasPrefix:@"assets-library://"] || [moviePath hasPrefix:@"ipod-library://"] ? [NSURL URLWithString:moviePath] :  [NSURL fileURLWithPath:moviePath];
}

+ (NSString*) getThumbnail:(NSString*)path
{
    NSString* thumbnailName = [NSString stringWithFormat:@"%@-thumbnail", [path lastPathComponent]];
    NSString* thumbnailPath = [NSHomeDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"Documents/%@.png", thumbnailName]];
    UIImage *thumbnail = NULL;
    //Only generate it if it doesn't exist
    //This method is badly named, it will check for ANY file, not just videos
    if(![VideoPlayerImplIOS videoExists:thumbnailPath])
    {
        //Try to get the thumbnail from Photo app first
        AVURLAsset *asset = [[[AVURLAsset alloc] initWithURL:[VideoPlayerImplIOS URLFromPath:path] options:nil] autorelease];
        AVAssetImageGenerator *generateImg = [[[AVAssetImageGenerator alloc] initWithAsset:asset] autorelease];
        NSError *error = NULL;
        CMTime time = CMTimeMake(1, 1);
        CGImageRef refImg = [generateImg copyCGImageAtTime:time actualTime:NULL error:&error];
        if(error == NULL)
        {
            thumbnail = [[[UIImage alloc] initWithCGImage:refImg] autorelease];
            //Fix the thumbnail orientation by rotating it (on iOS, the video can be rotated and there is a metadata that indicate that)
            AVAssetTrack *videoAssetTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
            CGAffineTransform videoTransform = videoAssetTrack != nil ? videoAssetTrack.preferredTransform : CGAffineTransformIdentity;
            thumbnail = [thumbnail imageRotatedWithTransform:videoTransform];
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
                    thumbnail = [itemArtwork imageWithSize:[itemArtwork bounds].size];
                }                
            }
        }
        
        if(thumbnail != NULL)
        {
            BOOL result = [UIImagePNGRepresentation(thumbnail) writeToFile:thumbnailPath options:NSDataWritingAtomic error:&error];
            NSLog(@"Write result for thumbnail %@ : %@, fullPath : %@", thumbnailName, (result ? @"OK" : @"Problem"), thumbnailPath);
            if(result)
            {
                std::string fullPath =  std::string(getenv("HOME")) + "/Documents/"+[thumbnailName UTF8String]+".png";
                CCTextureCache::sharedTextureCache()->removeTextureForKey(fullPath.c_str());
            }
            else
            {
                NSLog(@"Write error for thumbnail description: %@, reason: %@", [error localizedDescription], [error localizedFailureReason]);
                return NULL;
            }
        }
        else
        {
            thumbnailName = NULL;
        }
    }
    return thumbnailName;
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
    
    [player stop];
    [player.view removeFromSuperview];
	[player release];
    [path release];
    [super dealloc];
}

@end
