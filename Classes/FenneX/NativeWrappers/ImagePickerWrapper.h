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

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

typedef enum
{
    Camera = 0, // launch the camera apps and take a normal picture
    PhotoLibrary = 1, // launch the galleryApp to pick from it
    FileLibrary = 2, // on iOS, same as PhotoLibrary, on Android launch a file explorer app where the name is visible. The user can choose the app, so if it's a custom one, it can potentially return something wrong and not apply the filter.
}PickOption;
/**
 * pickImageFrom launch a platform-dependant widget to pick an image, either by taking a picture with the camera, or by using a library
 *
 * saveName: the file to save (can be in a subdirectory). Must ends with .png, as it will only generate PNG
 * location: the location the file must be saved in
 * pickOption: which widget to use to pick the image
 * width: max width of the image. It will be downscaled if necessary, by keeping ratio
 * height: max height of the image. It will be downscaled if necessary, by keeping ratio
 * identifier: an identifier which will be passed on result, to identify what the image was picked for
 *
 * will throw either
 * - ImagePicked, with file name, location and identifier, if the pick was successful
 * - ImagePickerCancelled if the user cancelled the widget
 **/
void pickImageFrom(const std::string& saveName, FileLocation location, PickOption pickOption, int width, int height, const std::string& identifier);
bool isCameraAvailable();

static inline void notifyImagePicked(std::string name, FileLocation location, std::string identifier)
{
    Value toSend = Value(ValueMap({{"Name", Value(name)}, {"Location", Value((int)location)}, {"Identifier", Value(identifier)}}));
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    DelayedDispatcher::eventAfterDelay("ImagePicked", toSend, 0.001);
#else
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ImagePicked", &toSend);
#endif
}
#endif



static inline void notifyImagePickCancelled()
{
    DelayedDispatcher::eventAfterDelay("ImagePickerCancelled", Value(), 0.01);
}


#endif
