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

#ifndef FenneX_AudioPickerWrapper_h
#define FenneX_AudioPickerWrapper_h

#include "FenneX.h"

USING_NS_FENNEX;


//check if it's a supported platform
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
bool pickSound(const std::string& promptText, const std::string& saveName, const std::string& identifier);
bool isAudioPickerExporting();
std::string audioPickerCurrentExport();
void stopAudioPickerExport();
#endif

static inline void notifySoundPicked(std::string name, std::string identifier)
{
    Value toSend = Value(ValueMap({{"Name", Value(name)}, {"Identifier", Value(identifier)}}));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("SoundPicked", &toSend);
}

static inline void notifySoundEncoded(std::string name, std::string identifier)
{
    Value toSend = Value(ValueMap({{"Name", Value(name)}, {"Identifier", Value(identifier)}}));
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("SoundEncoded", &toSend);
}

#endif
