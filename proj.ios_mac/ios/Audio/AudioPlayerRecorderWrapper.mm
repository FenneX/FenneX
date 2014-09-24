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
#import "AudioPlayerRecorderImplIOS.h"
#import "NSFileManager+ApplicationSupport.h"

NSString* getNSString(CCString* path)
{
    return [NSString stringWithFormat:@"%s", path->getCString()];
}

float AudioPlayerRecorder::getSoundDuration(CCString* file)
{
    return [AudioPlayerRecorderImpl getSoundDuration:[NSString stringWithUTF8String:file->getCString()]];
}

void AudioPlayerRecorder::setRecordEnabled(bool enabled)
{
    if(recordEnabled != enabled)
    {
        [[AudioPlayerRecorderImpl sharedAudio] setRecordEnabled:enabled];
        recordEnabled = enabled;
    }
}

void AudioPlayerRecorder::init()
{
    [AudioPlayerRecorderImpl sharedAudio];
}

bool AudioPlayerRecorder::isRecording()
{
    return link != NULL && [AudioPlayerRecorderImpl sharedAudio].isRecording;
}

bool AudioPlayerRecorder::isPlaying()
{
    return link != NULL && [AudioPlayerRecorderImpl sharedAudio].isPlaying;
}


void AudioPlayerRecorder::record(CCString* file, CCObject* linkTo)
{
    CCAssert(recordEnabled, "Record is disabled, enable it before starting to record");
	CCString* withExtension = ScreateF("%s.caf", file->getCString());
    if(linkTo == link && this->isRecording())
    {
        this->stopRecording();
    }
    else
    {
        if(linkTo == NULL)
        {
            linkTo = noLinkObject;
        }
        this->setLink(linkTo);
        this->setPath(withExtension);
        [[AudioPlayerRecorderImpl sharedAudio] startRecording];
    }
}

void AudioPlayerRecorder::stopRecording()
{
    CCAssert(recordEnabled, "Record is disabled, enable it before calling stopRecording");
    [[AudioPlayerRecorderImpl sharedAudio] stopRecording:getNSString(path)];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath(NULL);
}

float AudioPlayerRecorder::play(CCString* file, CCObject* linkTo, bool independent)
{
    if(independent)
    {
        return [[AudioPlayerRecorderImpl sharedAudio] playIndependentFile:[NSString stringWithUTF8String:file->getCString()]];
    }
    else
    {
        if(linkTo == link && this->isPlaying())
        {
            this->stopPlaying();
        }
        else
        {
            if(linkTo == NULL)
            {
                linkTo = noLinkObject;
            }
            if(this->isPlaying())
            {
                this->stopPlaying();
            }
            this->setLink(linkTo);
            this->setPath(file);
            [[AudioPlayerRecorderImpl sharedAudio] setPlayFile:[NSString stringWithUTF8String:file->getCString()]];
            return [[AudioPlayerRecorderImpl sharedAudio] play:0];
        }
    }
    return 0;
}

void AudioPlayerRecorder::stopPlaying(CCObject* obj)
{
    [[AudioPlayerRecorderImpl sharedAudio] stopPlaying];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath(NULL);
}

void AudioPlayerRecorder::fadeVolumeOut()
{
    [[AudioPlayerRecorderImpl sharedAudio] fadeVolumeOut];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath(NULL);
}

void AudioPlayerRecorder::play()
{
    [[AudioPlayerRecorderImpl sharedAudio] play];
}

void AudioPlayerRecorder::pause()
{
    [[AudioPlayerRecorderImpl sharedAudio] pause];    
}

void AudioPlayerRecorder::restart()
{
    [[AudioPlayerRecorderImpl sharedAudio] restart];    
}

void AudioPlayerRecorder::deleteFile(CCString* file)
{
    [[AudioPlayerRecorderImpl sharedAudio] deleteFile:getNSString(file)];
}

void AudioPlayerRecorder::setNumberOfLoops(int loops)
{
    [[AudioPlayerRecorderImpl sharedAudio] setNumberOfLoops:loops];
}

CCDictionary* AudioPlayerRecorder::getFileMetadata(CCString* path)
{
    NSString* file = [NSString stringWithUTF8String:path->getCString()];
    //try sound in bundle first
    NSURL* url = [[NSBundle mainBundle] URLForResource:[file stringByDeletingPathExtension] withExtension:@"mp3"];
    if(url == nil)
    {
        //try to load recorded sound if it's not in bundle
        url = [NSURL fileURLWithPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file]];
        if(url == nil)
        {
#if VERBOSE_AUDIO
            NSLog(@"Warning : file %@ does not exist", file);
#endif
            return NULL;
        }
    }
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:url options:nil];
    NSArray *metadata = [asset commonMetadata];
    CCDictionary* ccMetadata = Dcreate();
    ccMetadata->setObject(Icreate(getSoundDuration(path)), "Duration");
    for(AVMetadataItem* item in metadata) 
    {
        //Get the relevant metadata, currently title and artist
        if([[item commonKey] isEqual:@"title"])
        {
            ccMetadata->setObject(Screate([item stringValue].UTF8String), "Title");
        }
        else if([[item commonKey] isEqual:@"artist"])
        {
            ccMetadata->setObject(Screate([item stringValue].UTF8String), "Author");
        }
        //Uncomment to see what key/value are available in your files
        //CCLOG("key = %s, value = %s", [item commonKey].UTF8String, [item stringValue].UTF8String);
    }
    return ccMetadata;
}