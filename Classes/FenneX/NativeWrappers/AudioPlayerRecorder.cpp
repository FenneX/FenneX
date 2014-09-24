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
    path = NULL;
    noLinkObject = Node::create(); //the exact type isn't important, it just needs to be a Ref*
    noLinkObject->retain();
    recordEnabled = false;
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(AudioPlayerRecorder::playObject), "PlayObject", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(AudioPlayerRecorder::recordObject), "RecordObject", NULL);
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(AudioPlayerRecorder::stopPlaying), "PauseSound", NULL);
}


bool AudioPlayerRecorder::isRecordEnabled()
{
    return recordEnabled;
}

AudioPlayerRecorder::~AudioPlayerRecorder()
{
    CCNotificationCenter::sharedNotificationCenter()->removeAllObservers(this);
    if(path != NULL)
    {
        path->release();
        path = NULL;
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

void AudioPlayerRecorder::playObject(CCObject* obj)
{
    CCDictionary* infos = (CCDictionary*)obj;
    CCObject* linkTo = infos->objectForKey("Object");
    CCString* file = (CCString*) infos->objectForKey("File");
    if(linkTo == NULL)
    {
        linkTo = infos->objectForKey("Sender");
    }
    if(linkTo == NULL)
    {
        linkTo = noLinkObject;
    }
    if(file != NULL)
    {
        this->play(file, linkTo);
    }
}

void AudioPlayerRecorder::recordObject(CCObject* obj)
{
    CCAssert(recordEnabled, "Record is disabled, enable it before starting to record");
    if(this->isPlaying())
    {
        this->stopPlaying();
    }
    CCDictionary* infos = (CCDictionary*)obj;
    CCObject* linkTo = infos->objectForKey("Object");
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
    CCString* file = ScreateF("%s_%d-%02d-%02d_%02d.%02d.%02d", getPackageIdentifier(), timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    if(file != NULL)
    {
        this->record(file, linkTo);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("RecordingStarted", DcreateP(linkTo, Screate("Object"), oldFile, Screate("OldFile"), NULL));
    }
}

CCObject* AudioPlayerRecorder::getLink()
{
    return link;
}

CCString* AudioPlayerRecorder::getPath()
{
    return path;
}

CCString* AudioPlayerRecorder::getPathWithoutExtension()
{
    std::string pathStd = path->getCString();
    if(hasEnding(pathStd, SOUND_EXTENSION))
    {
        return Screate(pathStd.substr(0, pathStd.length() - std::string(SOUND_EXTENSION).length()).c_str());
    }
    return path;
}

void AudioPlayerRecorder::setPath(CCString* value)
{
    if(path != value)
    {
#if VERBOSE_AUDIO
        if(value != NULL)
        {
            CCLOG("Changing audio path to : %s", value->getCString());
        }
        else
        {
            CCLOG("Chaning audio path to NULL");
        }
#endif
        if(path != NULL)
        {
            path->release();
        }
        path = value;
        if(path != NULL)
        {
            path->retain();
        }
    }
}

void AudioPlayerRecorder::setLink(CCObject* value)
{
    if(value != link)
    {
        this->stopAll();
        link = value;
    }
}
