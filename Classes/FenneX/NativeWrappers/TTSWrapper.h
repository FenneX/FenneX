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

#ifndef __FenneX__TTSWrapper_h
#define __FenneX__TTSWrapper_h
#include "cocos2d.h"
#include "Shorteners.h"
#include "FenneX.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif
USING_NS_FENNEX;

//TTS requires iOS 7.0+, and will work on any Android. It is advised to update TTS Engine on Android
void initTTS();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
jobject getInstance();
#endif
//SpeechID is currently not properly implemented(will always return last speechID)
bool speakText(std::vector<std::string> text, int speechID = -1);
void stopSpeakText();
bool isSpeaking();

//Must be called before playing a TTS. The rate is global for all subsequent TTS
float getTTSPlayRate();
void setTTSPlayRate(float rate);

std::string getTTSEngineName();

static inline void notifyTTSDone(int speechID)
{
    DelayedDispatcher::eventAfterDelay("TTSDone", DcreateP(Icreate(speechID), Screate("Identifier"), NULL), 0.01);
}

#endif /* defined(__FenneX__TTSWrapper_h) */
