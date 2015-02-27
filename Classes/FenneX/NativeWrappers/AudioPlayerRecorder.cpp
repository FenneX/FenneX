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
#include "Logs.h"
#include "NativeUtility.h"

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
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    Ref* linkTo = infos->objectForKey("Object");
    CCString* file = (CCString*) infos->objectForKey("File");
    if(linkTo == NULL)
    {
        linkTo = infos->objectForKey("Sender");
        if(isKindOfClass(linkTo, CCInteger))
        {
            RawObject* target = GraphicLayer::sharedLayer()->getById(TOINT(linkTo));
            if(target != NULL) linkTo = target;
        }
    }
    if(linkTo == NULL)
    {
        linkTo = noLinkObject;
    }
    if(file != NULL)
    {
        this->play(file->_string, linkTo);
    }
}

void AudioPlayerRecorder::recordObject(EventCustom* event)
{
    CCAssert(recordEnabled, "Record is disabled, enable it before starting to record");
    if(this->isPlaying())
    {
        this->stopPlaying();
    }
    CCDictionary* infos = (CCDictionary*)event->getUserData();
    Ref* linkTo = infos->objectForKey("Object");
    CCString* oldFile = (CCString*) infos->objectForKey("File");
    if(linkTo == NULL)
    {
        linkTo = infos->objectForKey("Sender");
    }
    if(linkTo == NULL)
    {
        linkTo = noLinkObject;
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
        this->record(file, linkTo);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("RecordingStarted", DcreateP(linkTo, Screate("Object"), oldFile, Screate("OldFile"), NULL));
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

void AudioPlayerRecorder::setPath(std::string value)
{
    if(path.compare(value) != 0)
    {
#if VERBOSE_AUDIO
        if(value.length() > 0)
        {
            CCLOG("Changing audio path to : %s", value.c_str());
        }
        else
        {
            CCLOG("Chaning audio path to NULL");
        }
#endif
        path = value;
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
