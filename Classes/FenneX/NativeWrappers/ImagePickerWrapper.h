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

#ifndef FenneX_ImagePickerWrapper_h
#define FenneX_ImagePickerWrapper_h

#include "FenneX.h"
USING_NS_FENNEX;

// Pick an image using a widget (Intent on Android, UIImagePicker on iOS). On Android, default to a regular File explorer with image mimetype filter instead of specific photo library, because photo libraries generally have usability issues
void pickImageWithWidget();

// Taking a photo with widget in only available on iOS, as Android Intent have a fatal flaw we don't want to work around: the app can be killed because of OOM when the Intent takes the picture
// This definitly happens often on some Android devices. You should instead use CameraHandler to take the picture in the app
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
void takePhotoWithWidget();
#endif

bool isCameraAvailable();

static inline void notifyImagePicked(const std::string& fullpath)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    //Android always save permanent files in the media directory that cocos2d-x is able to access
    Value toSend = Value(ValueMap({{"Path", Value(fullpath)}, {"Temporary", Value(false)}}));
    DelayedDispatcher::eventAfterDelay("ImagePicked", toSend, 0.001);
#else
    //iOS always save temporary files because cocos2d-x isn't able to directly access library files
    Value toSend = Value(ValueMap({{"Path", Value(fullpath)}, {"Temporary", Value(true)}}));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ImagePicked", &toSend);
#endif
}



static inline void notifyImagePickCancelled()
{
    DelayedDispatcher::eventAfterDelay("ImagePickerCancelled", Value(), 0.01);
}


#endif
