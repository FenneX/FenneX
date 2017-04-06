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

#include "FenneX.h"

USING_NS_FENNEX;

//Warning : picking a video from camera will launch VideoRecorder on background. You'll need to manually stop it with a foreground button
//the zone it should be recording in is specified when starting the preview, otherwise it's full screen
//If you start a videoRecordPreview, you must either stop it or pick a video from camera, then stop the recording
//Warning : picking a video from camera is incompatible with AudioPlayerRecorder
//Requires permissions on Android :
//  <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
//  <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
//  <uses-permission android:name="android.permission.RECORD_VIDEO" />
//  <uses-permission android:name="android.permission.CAMERA" />

//Before actually starting a video record, you should show a Preview.
void startVideoRecordPreview(Vec2 position, cocos2d::Size size);
void stopVideoRecordPreview();
//You can start a video recording directly after a Preview. When you stop it, a VideoPicked notify will be thrown
void startVideoRecording();
void stopVideoRecording();

//Will cancel either preview or recording (depending which mode you are on) without generating a file and a notification if you are recording. Return true if something was cancelled
//Pass true to get a VideoRecordingCancelled when it's done
bool cancelRecording(bool notify = true);

bool pickVideoFromLibrary(const std::string& saveName);

bool pickVideoFromCamera(const std::string& saveName);

//Will start the process of getting all videos path, names and duration, which will be notified with VideoFound, VideoNameResolved and VideoDurationAvailable (from VideoPlayer)
//Current implementation is in the same thread for iOS, and in a different thread on Android (but still using all the resources)
//Will throw a GetAllVideosFinished notification when it ends
void getAllVideos();

static inline void notifyVideoPicked(std::string name)
{
    DelayedDispatcher::eventAfterDelay("VideoPicked", DcreateP(Screate(name), Screate("Name"), NULL), 0.01);
}

static inline void notifyVideoFound(std::string fullPath)
{
    DelayedDispatcher::eventAfterDelay("VideoFound", DcreateP(Screate(fullPath), Screate("Path"), NULL), 0.01);
}

//This notification will be sent after VideoFound (it is necesary on iOS to be there because the reference URL is currently not saved) : you should save the name yourself if you need it
static inline void notifyVideoName(std::string path, std::string name)
{
    DelayedDispatcher::eventAfterDelay("VideoNameResolved", DcreateP(Screate(name), Screate("Name"), Screate(path), Screate("Path"), NULL), 0.01);
}

static inline void notifyRecordingCancelled()
{
    DelayedDispatcher::eventAfterDelay("VideoRecordingCancelled", Dcreate(), 0.01);
}

static inline void notifyGetAllVideosFinished()
{
    DelayedDispatcher::eventAfterDelay("GetAllVideosFinished", Dcreate(), 0.01);
}

static inline void notifyVideoPickCancelled()
{
    DelayedDispatcher::eventAfterDelay("VideoPickerCancelled", Dcreate(), 0.01);
}

#endif
