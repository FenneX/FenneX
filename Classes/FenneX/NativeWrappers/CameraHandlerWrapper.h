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

#ifndef FenneX_CameraHandler_h
#define FenneX_CameraHandler_h

#include "cocos2d.h"
#include "DelayedDispatcher.h"
#include "FileUtility.h"

USING_NS_FENNEX;


//Before actually starting a video record or when preparing to take a picture, you should show a Preview. Taking pictures on Android only don't need audio. That parameter is ignored on iOS
void startCameraPreview(Vec2 position, cocos2d::Size size, bool front = false, bool requiresAudio = true);
void stopCameraPreview();
//You should also show a switch camera button if there are several cameras available
bool canSwitchCamera();
//You can start a video recording directly after a Preview. When you stop it, a VideoPicked notify will be thrown
void startVideoRecording();
//Save in the Movies folder by default on Android and Camera Roll on iOS. Use stopVideoRecordingSaveTo to save somewhere else (default behavior kept for compatibility)
void stopVideoRecording();

//Will cancel either preview or recording (depending which mode you are on) without generating a file and a notification if you are recording. Return true if something was cancelled
//Pass true to get a VideoRecordingCancelled when it's done
bool cancelRecording(bool notify = true);


#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//Directly capture a picture inapp. Requires starting and stoping video record preview
//This option is only available on Android, since using Intent present a major issue: on some devices, the app is killed when the camera app takes the picture
//to free some RAM, and implemeting a mechanism so that the app is able to resume is costly
//Instead, you can re-locate the capture in the app, so Android won't kill it.
//IMPORTANT: when getting notified a picture taken, you must delete the temporary image being passed, but after a short delay (~1 sec) so that
//the native code has the opportunity to save the file to Camera roll
//This is done asynchronously so that the UI is notified earlier of the file being available, to make it more responsive
void capturePicture();

//When switching camera, the interface should be locked until the CameraSwitched event is fired.
//Capturing a picture during the switch causes a crash otherwise
void switchCamera();

//Same as stopVideoRecording except it saves to a specific path. It must include the .mp4 extension. Other formats are not supported right now.
void stopVideoRecordingSaveTo(std::string path, FileLocation location);
#endif

static inline void notifyPictureTaken(std::string path)
{
    DelayedDispatcher::eventAfterDelay("PictureTaken", Value(ValueMap({{"Path", Value(path)}})), 0.01);
}

static inline void notifyRecordingCancelled()
{
    DelayedDispatcher::eventAfterDelay("VideoRecordingCancelled", Value(), 0.01);
}

static inline void notifyCameraSwitched()
{
    DelayedDispatcher::eventAfterDelay("CameraSwitched", Value(), 0.01);
}

#endif
