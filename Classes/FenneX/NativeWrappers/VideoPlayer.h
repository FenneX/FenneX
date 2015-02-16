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

#ifndef FenneX_VideoPlayer_h
#define FenneX_VideoPlayer_h

#include "FenneX.h"
USING_NS_CC;
USING_NS_FENNEX;

//On Android, you must reference videos URI from the Bundle in the main app.java getURIFromFilename
//Warning : there is a small difference :
//on iOS you'll need to play a video after creating it
//on Android, a video will autoplay after prepare finish
class VideoPlayer : public Ref
{
public:
    VideoPlayer(std::string file, Vec2 position, cocos2d::Size size, bool front = true, bool loop = true);
    ~VideoPlayer();
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //On Android, the default implementation use a VideoView
    //You can use LibVLC (which requires a different compilation script and additional files not in FenneX)
    //LibVLC allow to change PlaybackRate, which is not possible with VideoView
    //It also support more formats
    static void setUseVLC(bool useVLC);
#endif
    void play();
    void pause();
    void stop();
    float getPlaybackRate();
    void setPlaybackRate(float rate);
    void setHideOnPause(bool hide);
    void setFullscreen(bool fullscreen, bool animated = true);
    bool isFullscreen();
    
    //All durations are in seconds
    float getDuration();
    float getPosition();
    void setPosition(float position);
    
    //Return the path of the thumbnail (use getLocalPath to get the absolute path)
    //May return NULL if there was a problem generating the thumbnail
    static std::string getThumbnail(const std::string& path);
    
    //On iOS, always returns true for external videos and will notify VideoExists/VideoRemoved with a CCDictionary containing Path key with a CCString
    //For trimmed video (picked from library) on iOS and all video on Android, directly return the right value
    static bool videoExists(const std::string& file);
private:
//TODO : refactor to have a cross-platform way to do that (use ui::EditBox as an example)
    void* delegate;
};

static inline void notifyVideoDurationAvailable(const char* path, float duration)
{
    DelayedDispatcher::eventAfterDelay("VideoDurationAvailable", DcreateP(Screate(path), Screate("Path"), Fcreate(duration), Screate("Duration"), NULL), 0.01);
}

static inline void notifyVideoEnded(const char* path)
{
    DelayedDispatcher::eventAfterDelay("VideoEnded", DcreateP(Screate(path), Screate("Path"), NULL), 0.01);
}

static inline void notifyVideoExists(const char* path)
{
    DelayedDispatcher::eventAfterDelay("VideoExists", DcreateP(Screate(path), Screate("Path"), NULL), 0.01);
}

static inline void notifyVideoRemoved(const char* path)
{
    DelayedDispatcher::eventAfterDelay("VideoRemoved", DcreateP(Screate(path), Screate("Path"), NULL), 0.01);
}

#endif /* defined(__FenneX__VideoPlayer__) */
