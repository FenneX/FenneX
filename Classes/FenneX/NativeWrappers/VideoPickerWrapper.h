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

#ifndef FenneX_VideoPickerWrapper_h
#define FenneX_VideoPickerWrapper_h

#include "cocos2d.h"
#include "FileUtility.h"
#include "DelayedDispatcher.h"

USING_NS_FENNEX;


void pickVideoFromLibrary(const std::string& saveName, FileLocation location);
void pickVideoFromCamera(const std::string& saveName, FileLocation location);

//Will start the process of getting all videos path, names and duration, which will be notified with VideoFound, VideoNameResolved and VideoDurationAvailable (from VideoPlayer)
//Current implementation is in the same thread for iOS, and in a different thread on Android (but still using all the resources)
//Will throw a GetAllVideosFinished notification when it ends
void getAllVideos();

static inline void notifyVideoPicked(std::string name, FileLocation location)
{
    DelayedDispatcher::eventAfterDelay("VideoPicked", Value(ValueMap({{"Name", Value(name)}, {"Location", Value((int)location)}})), 0.01);
}

static inline void notifyVideoFound(std::string fullPath)
{
    DelayedDispatcher::eventAfterDelay("VideoFound", Value(ValueMap({{"Path", Value(fullPath)}})), 0.01);
}

//This notification will be sent after VideoFound (it is necesary on iOS to be there because the reference URL is currently not saved) : you should save the name yourself if you need it
static inline void notifyVideoName(std::string path, std::string name)
{
    DelayedDispatcher::eventAfterDelay("VideoNameResolved", Value(ValueMap({{"Name", Value(name)}, {"Path", Value(path)}})), 0.01);
}

static inline void notifyGetAllVideosFinished()
{
    DelayedDispatcher::eventAfterDelay("GetAllVideosFinished", Value(), 0.01);
}

static inline void notifyVideoPickCancelled()
{
    DelayedDispatcher::eventAfterDelay("VideoPickerCancelled", Value(), 0.01);
}

#endif
