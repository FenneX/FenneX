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

#include "TTSWrapper.h"
#import "TTS.h"

void initTTS()
{
    [TTS sharedTTS];
}

bool speakText(std::vector<std::string> text, int speechID)
{
    NSMutableArray* nsText = [NSMutableArray array];
    for(std::string t : text)
    {
        [nsText addObject:[NSString stringWithUTF8String:t.c_str()]];
    }
    [[TTS sharedTTS] speakText:nsText callbackID:speechID];
    return true;
}

void stopSpeakText()
{
    [[TTS sharedTTS] stopSpeakText];
}

bool isSpeaking()
{
    return [[TTS sharedTTS] isSpeaking];
}

float getTTSPlayRate()
{
    return [TTS sharedTTS].playRate;
}

void setTTSPlayRate(float rate)
{
    [TTS sharedTTS].playRate = rate;
}

std::string getTTSEngineName()
{
    // We don't need anything realy here, because ios tts engine doesn't have a package name like android has
    return "ios.tts.engine";
}

std::map<std::string, std::string> getTTSEngines()
{
    return {{"ios.tts.engine", "iOS Text To Speech"}};
}

void setTTSEngine(std::string engineName){}
