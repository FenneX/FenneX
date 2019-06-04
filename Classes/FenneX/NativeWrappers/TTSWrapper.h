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

//When a particular voice WPM (words per minute) is not known, use this value as default
//It's better to have a slower rather than faster default speech WPM, so that text is not cut off
#define DEFAULT_SPEECH_WPM 130

//TTS requires iOS 7.0+, and will work on any Android. It is advised to update TTS Engine on Android
void initTTS();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
jobject getInstance();
void initTTS(std::string engineName);
#endif
//SpeechID is currently not properly implemented(will always return last speechID)
bool speakText(std::vector<std::string> text, int speechID = -1, float volume = 1);
void stopSpeakText();
bool isSpeaking();
//It is not possible to get exact duration like AudioPlayer does, but we often don't need a precise duration
//Takes into account both Voice WPM and current Play Rate, and return a duration in seconds
float estimateSpeechDuration(std::string text);
float getCurrentVoiceWPM();

//Must be called before playing a TTS. The rate is global for all subsequent TTS
float getTTSPlayRate();
void setTTSPlayRate(float rate);

std::string getTTSEngineName();

// Return mapof all the text to speech engine, name as key (identifier), label as values
std::map<std::string, std::string> getTTSEngines();

// Set the using tts engine for the running app
void setTTSEngine(std::string engineName);

static inline void notifyTTSDone(int speechID)
{
    DelayedDispatcher::eventAfterDelay("TTSDone", Value(ValueMap({{"Identifier", Value(speechID)}})), 0.01);
}

#endif /* defined(__FenneX__TTSWrapper_h) */
