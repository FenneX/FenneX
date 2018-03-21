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
#import <AVKit/AVKit.h>
#import <CoreMedia/CoreMedia.h>
#import <MediaPlayer/MediaPlayer.h>

@interface VideoPlayerImplIOS : NSObject
{
    AVPlayerViewController* playerController;
    UIInterfaceOrientation currentOrientation;
    CGPoint _position;
    CGSize _size;
    BOOL hideOnPause;
    BOOL isFullScreen;
    NSString* path;
    float desiredPlaybackRate;
    BOOL _loop;
}

@property (nonatomic) float playbackRate;
@property (nonatomic, assign) BOOL hideOnPause;
@property (nonatomic, readonly) BOOL fullscreen;
@property (nonatomic, readonly) float duration;
@property (nonatomic) float position;
@property (nonatomic, readonly) BOOL loop;

//The file extension will be auto-detected. If there is no extension, ".mov" will be assumed
- (id) initWithPlayFile:(NSString*)file position:(CGPoint)position size:(CGSize)size front:(BOOL)front loop:(BOOL)loop;
- (void) setPlayerPosition:(CGPoint)position size:(CGSize)size animated:(BOOL)animated;
- (void) play;
- (void) pause;
- (void) stop;

- (void) setFullscreen:(BOOL)fullscreen animated:(BOOL)animated;
- (void) setMuted:(BOOL)muted;

+ (BOOL) getThumbnail:(NSString*)path thumbnailName:(NSString*)thumbnailName;
+ (CGSize) getVideoSize:(NSString*)path;
+ (BOOL) videoExists:(NSString*)path;

@end
