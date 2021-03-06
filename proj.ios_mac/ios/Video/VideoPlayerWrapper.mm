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

VideoPlayer::VideoPlayer(std::string file, Vec2 position, cocos2d::Size size, bool front, bool loop)
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


void VideoPlayer::setPlayerPosition(Vec2 position, cocos2d::Size size, bool animated)
{
    cocos2d::GLView *glview = cocos2d::Director::getInstance()->getOpenGLView();
    CCEAGLView *eaglview = (CCEAGLView*) glview->getEAGLView();
    float scaleFactor = [eaglview contentScaleFactor];
    [TYPED_DELEGATE setPlayerPosition:CGPointMake(position.x / scaleFactor,
                                                  position.y / scaleFactor)
                                 size:CGSizeMake(size.width/ scaleFactor,
                                                 size.height/ scaleFactor)
                             animated:animated];
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

void VideoPlayer::setMuted(bool muted)
{
    TYPED_DELEGATE.muted = muted;
}

std::string VideoPlayer::getScreenshot(const std::string& path, FileLocation videoLocation, const std::string& screenshotPath, FileLocation screenshotLocation)
{
    std::string fileName = screenshotPath;
    if(fileName.empty())
    {
        fileName = path;
        if(videoLocation == FileLocation::Absolute && screenshotLocation != FileLocation::Absolute && path.find_last_of('/') != std::string::npos)
        { // If we are not using absolute for screenshot but we use it for video, that mean we have a path to parse
            fileName = path.substr(path.find_last_of('/') + 1);
        }
        fileName += "-thumbnail";
    }
    fileName += ".png";
    NSString* screenshotPathString = [NSString stringWithUTF8String:getFullPath(fileName, screenshotLocation).c_str()];
    BOOL result = [VideoPlayerImplIOS getScreenshot:[NSString stringWithUTF8String:getFullPath(path, videoLocation).c_str()]
                                     screenshotName:screenshotPathString];
    return result ? [screenshotPathString UTF8String] : "";
}

cocos2d::Size VideoPlayer::getVideoSize(const std::string& path, FileLocation location)
{
    CGSize size = [VideoPlayerImplIOS getVideoSize:[NSString stringWithUTF8String:getFullPath(path, location).c_str()]];
    return cocos2d::Size(size.width, size.height);
}

bool VideoPlayer::videoExists(const std::string& file)
{
    return [VideoPlayerImplIOS videoExists:[NSString stringWithUTF8String:file.c_str()]];
}
