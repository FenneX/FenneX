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

#include "AudioPlayerRecorder.h"
#include "NativeUtility.h"
#include "GraphicLayer.h"

USING_NS_FENNEX;

static AudioPlayerRecorder *s_SharedRecorder = NULL;

AudioPlayerRecorder* AudioPlayerRecorder::sharedRecorder(void)
{
    if (!s_SharedRecorder)
    {
        s_SharedRecorder = new AudioPlayerRecorder();
        s_SharedRecorder->init();
    }
    
    return s_SharedRecorder;
}

AudioPlayerRecorder::AudioPlayerRecorder()
{
    link = NULL;
    path = "";
    loops = 0;
    noLinkObject = Node::create(); //the exact type isn't important, it just needs to be a Ref*
    noLinkObject->retain();
    recordEnabled = false;
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("PlayObject", std::bind(&AudioPlayerRecorder::playObject, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("RecordObject", std::bind(&AudioPlayerRecorder::recordObject, this, std::placeholders::_1)));
    listeners.pushBack(Director::getInstance()->getEventDispatcher()->addCustomEventListener("PauseSound", std::bind(&AudioPlayerRecorder::stopPlaying, this, std::placeholders::_1)));
}


bool AudioPlayerRecorder::isRecordEnabled()
{
    return recordEnabled;
}

AudioPlayerRecorder::~AudioPlayerRecorder()
{
    for(EventListenerCustom* listener : listeners)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
    }
    listeners.clear();
    if(!path.empty())
    {
        path = "";
    }
    s_SharedRecorder = NULL;
}

void AudioPlayerRecorder::stopAll()
{
    if(this->isRecording())
    {
        this->stopRecording();
    }
    else if(this->isPlaying())
    {
        this->stopPlaying();
    }
}

void AudioPlayerRecorder::playObject(EventCustom* event)
{
    ValueMap infos = ((Value*)event->getUserData())->asValueMap();
    RawObject* linkTo = NULL;
    if(isValueOfType(infos["Object"], INTEGER))
    {
        linkTo = GraphicLayer::sharedLayer()->first(infos["Object"].asInt());
    }
    if(isValueOfType(infos["Sender"], INTEGER))
    {
        linkTo = GraphicLayer::sharedLayer()->first(infos["Sender"].asInt());
    }
    if(isValueOfType(infos["File"], STRING))
    {
        this->play(infos["File"].asString(), linkTo);
    }
}

void AudioPlayerRecorder::recordObject(EventCustom* event)
{
    CCAssert(recordEnabled, "Record is disabled, enable it before starting to record");
    if(this->isPlaying())
    {
        this->stopPlaying();
    }
    ValueMap infos = ((Value*)event->getUserData())->asValueMap();
    RawObject* linkTo = NULL;
    
    if(isValueOfType(infos["Object"], INTEGER))
    {
        linkTo = GraphicLayer::sharedLayer()->first(infos["Object"].asInt());
    }
    if(isValueOfType(infos["Sender"], INTEGER))
    {
        linkTo = GraphicLayer::sharedLayer()->first(infos["Sender"].asInt());
    }
    
    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime (&rawtime);

    // format : "%s_%d-%02d-%02d_%02d.%02d.%02d"
    std::string file = std::string(getPackageIdentifier()) + "_"
                        + std::to_string(timeinfo->tm_year+1900) + "-"
                        + std::to_string(timeinfo->tm_mon+1).substr(0,2) + "-"
                        + std::to_string(timeinfo->tm_mday).substr(0,2) + "_"
                        + std::to_string(timeinfo->tm_hour).substr(0,2) + "."
                        + std::to_string(timeinfo->tm_min).substr(0,2) + "."
                        + std::to_string(timeinfo->tm_sec).substr(0,2);
    
    if(!file.empty())
    {
        std::string oldFile = infos["File"].asString();
        FileLocation loc = FileLocation::ApplicationSupport; //Legacy: used to be targeted to ApplicationSupport
        if(valueMapContains(infos, "Location", INTEGER))
        {
            loc = (FileLocation)infos["Location"].asInt();
        }
        this->record(file, loc, linkTo);
        ValueMap map = {{"OldFile", Value(oldFile)}};
        if(linkTo != NULL) map["Object"] = linkTo->getID();
        Value toSend = Value(map);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("RecordingStarted", &toSend);
    }
}

Ref* AudioPlayerRecorder::getLink()
{
    return link;
}

std::string AudioPlayerRecorder::getPath()
{
    return path;
}

std::string AudioPlayerRecorder::getPathWithoutExtension()
{
    std::string pathStd = path;
    if(hasEnding(pathStd, SOUND_EXTENSION))
    {
        return pathStd.substr(0, pathStd.length() - std::string(SOUND_EXTENSION).length());
    }
    return path;
}


void AudioPlayerRecorder::setNumberOfLoops(int loops, float pauseBetween)
{ //Looping is done is common code because pauseBetween isn't supported by any platform
    //Looping itself is fully supported on iOS, partially on MediaPlayer (only infinite) and partially on LibVLC (using MediaList, which is more of a workaround than real looping)
    this->loops = loops;
    this->pauseBetween = pauseBetween;
}

void AudioPlayerRecorder::onSoundEnded()
{
    if(loops > 0 || loops == -1)
    {
        if(pauseBetween <= 0)
        {
            this->restart();
        }
        else
        {
            interruptLoop = false;
            DelayedDispatcher::funcAfterDelay([this](EventCustom* event)
            {
                if(!interruptLoop)
                {
                    this->restart();
                }
            }, Value(), pauseBetween);
        }
        if(loops != -1)
        {
            loops--;
        }
    }
    else
    {
        pauseBetween = 0;
        DelayedDispatcher::eventAfterDelay("PlayingSoundEnded", Value(), 0.01);
    }
}

void AudioPlayerRecorder::setPath(const std::string& value, FileLocation loc)
{
    if(path.compare(value) != 0)
    {
#if VERBOSE_AUDIO
        if(value.length() > 0)
        {
            log("Changing audio path to : %s", value.c_str());
        }
        else
        {
            log("Chaning audio path to NULL");
        }
#endif
        path = value;
        location = loc;
    }
}

void AudioPlayerRecorder::setLink(Ref* value)
{
    if(value != link)
    {
        this->stopAll();
        link = value;
    }
}
