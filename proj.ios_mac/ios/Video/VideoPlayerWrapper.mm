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

#include "VideoPlayer.h"
#include "VideoPlayerImplIOS.h"
#import "platform/ios/CCEAGLView-ios.h"

#define TYPED_DELEGATE ((VideoPlayerImplIOS*)delegate)

VideoPlayer::VideoPlayer(std::string file, CCPoint position, CCSize size, bool front, bool loop)
{
    cocos2d::GLView *glview = cocos2d::Director::getInstance()->getOpenGLView();
    CCEAGLView *eaglview = (CCEAGLView*) glview->getEAGLView();
    float scaleFactor = [eaglview contentScaleFactor];
    delegate = [[VideoPlayerImplIOS alloc] initWithPlayFile:[NSString stringWithUTF8String:file.c_str()]
                                                   position:CGPointMake(position.x / scaleFactor,
                                                                        position.y / scaleFactor)
                                                       size:CGSizeMake(size.width/ scaleFactor,
                                                                       size.height/ scaleFactor)
                                                      front:front
                                                       loop:loop];
}

VideoPlayer::~VideoPlayer()
{
    [TYPED_DELEGATE release];
    delegate = nil;
}

void VideoPlayer::play()
{
    [TYPED_DELEGATE play];
}

void VideoPlayer::pause()
{
    [TYPED_DELEGATE pause];
}

void VideoPlayer::stop()
{
    [TYPED_DELEGATE stop];
}

float VideoPlayer::getPlaybackRate()
{
    return TYPED_DELEGATE.playbackRate;
}

void VideoPlayer::setPlaybackRate(float rate)
{
    TYPED_DELEGATE.playbackRate = rate;
}

void VideoPlayer::setHideOnPause(bool hide)
{
    TYPED_DELEGATE.hideOnPause = hide;
}

void VideoPlayer::setFullscreen(bool fullscreen, bool animated)
{
    [TYPED_DELEGATE setFullscreen:fullscreen animated:animated];
}

bool VideoPlayer::isFullscreen()
{
    return TYPED_DELEGATE.fullscreen;
}

float VideoPlayer::getDuration()
{
    return TYPED_DELEGATE.duration;
}

float VideoPlayer::getPosition()
{
    return TYPED_DELEGATE.position;
}

void VideoPlayer::setPosition(float position)
{
    [TYPED_DELEGATE setPosition:position];
}

std::string VideoPlayer::getThumbnail(const std::string& path)
{
    NSString* thumbnailPath = [VideoPlayerImplIOS getThumbnail:[NSString stringWithUTF8String:path.c_str()]];
    return thumbnailPath != nil ? [thumbnailPath UTF8String] : "";
}

bool VideoPlayer::isValidVideo(const std::string& filePath)
{
    return true;
}

bool VideoPlayer::videoExists(const std::string& file)
{
    return [VideoPlayerImplIOS videoExists:[NSString stringWithUTF8String:file.c_str()]];
}
