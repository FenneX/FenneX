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

/*
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <jni.h>
void Java_org_cocos2dx_socialhandy_FenneX_notifyImagePickedWrapper(JNIEnv* env, jobject thiz, jstring name, jstring identifier);
#endif*/

//this method have to be implemented in each platform. The parameter is the local path at which the image will be saved
//return false if there is a problem
//The image picker will send an ImagePicked notification

//The picker can include a thumbnail at the desired scale, which will have a -thumbnail prefix (before .png)
//If the scale is negative, no thumbnail will be generated. Generally, if you disable rescale, you probably won't need a thumbnail

//check if it's a supported platform
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
bool pickImageFrom(const std::string& saveName, bool useCamera, int width, int height, const std::string& identifier, bool rescale = true, float thumbnailScale = -1);
bool isCameraAvailable();

static inline void notifyImagePicked(std::string name, std::string identifier)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    DelayedDispatcher::eventAfterDelay("ImagePicked", DcreateP(Screate(name), Screate("Name"), Screate(identifier), Screate("Identifier"), NULL), 0.001);
#else
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ImagePicked", DcreateP(Screate(name), Screate("Name"), Screate(identifier), Screate("Identifier"), NULL) );
#endif
}
#endif


#endif
