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

#ifndef FenneX_LocalNotificationWrapper_h
#define FenneX_LocalNotificationWrapper_h

#include "cocos2d.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <jni.h>
#endif
USING_NS_CC;

//Warning : only sound in bundle are supported
//include CallbackEvent in the userInfo to have an event if the user do the action
void scheduleNotification(float timeFromNow, const std::string& alertBody, const std::string& alertAction, const std::string& soundName, CCDictionary* userInfo);

void cancelAllNotifications();

//Warning : on Android, notifications will not happen if the app is shut down
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

void stopService();

void notifyNotifClicked(jobjectArray array);

void notifyDeletePListFiles();

#endif

#endif
