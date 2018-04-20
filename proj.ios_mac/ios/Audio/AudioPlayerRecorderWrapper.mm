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

NSString* getNSString(std::string path)
{
    return [NSString stringWithFormat:@"%s", path.c_str()];
}

float AudioPlayerRecorder::getSoundDuration(const std::string& file)
{
    return [AudioPlayerRecorderImpl getSoundDuration:[NSString stringWithUTF8String:file.c_str()]];
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

std::string AudioPlayerRecorder::getSoundsSavePath()
{
    NSString *iosString = [[NSFileManager defaultManager] applicationSupportDirectory];
    return [iosString UTF8String];
}

void AudioPlayerRecorder::record(const std::string& file, FileLocation loc, Ref* linkTo)
{
    CCAssert(recordEnabled, "Record is disabled, enable it before starting to record");
	std::string withExtension = file + ".caf";
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
        this->setPath(withExtension, loc);
        [[AudioPlayerRecorderImpl sharedAudio] startRecording];
    }
}

void AudioPlayerRecorder::stopRecording()
{
    CCAssert(recordEnabled, "Record is disabled, enable it before calling stopRecording");
    [[AudioPlayerRecorderImpl sharedAudio] stopRecording:getNSString(getFullPath(path, location))];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

float AudioPlayerRecorder::play(const std::string& file, Ref* linkTo, bool independent, float volume)
{
    if(independent)
    {
        return [[AudioPlayerRecorderImpl sharedAudio] playIndependentFile:[NSString stringWithUTF8String:file.c_str()]
                                                                   volume:volume];
    }
    else
    {
        interruptLoop = true;
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
            [[AudioPlayerRecorderImpl sharedAudio] setPlayFile:[NSString stringWithUTF8String:file.c_str()]];
            return [[AudioPlayerRecorderImpl sharedAudio] play:0
                                                        volume:volume];
        }
    }
    return 0;
}

void AudioPlayerRecorder::stopPlaying(EventCustom* event)
{
    interruptLoop = true;
    [[AudioPlayerRecorderImpl sharedAudio] stopPlaying];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

void AudioPlayerRecorder::fadeVolumeOut()
{
    [[AudioPlayerRecorderImpl sharedAudio] fadeVolumeOut];
    link = NULL; //don't call setLink to avoid infinite recursion
    this->setPath("");
}

void AudioPlayerRecorder::play()
{
    [[AudioPlayerRecorderImpl sharedAudio] play];
}

void AudioPlayerRecorder::pause()
{
    interruptLoop = true;
    [[AudioPlayerRecorderImpl sharedAudio] pause];    
}

void AudioPlayerRecorder::restart()
{
    interruptLoop = true;
    [[AudioPlayerRecorderImpl sharedAudio] restart];    
}

float AudioPlayerRecorder::getPlaybackRate()
{
    return [AudioPlayerRecorderImpl sharedAudio].playbackRate;
}

void AudioPlayerRecorder::setPlaybackRate(float rate)
{
    [AudioPlayerRecorderImpl sharedAudio].playbackRate = rate;
}

ValueMap AudioPlayerRecorder::getFileMetadata(const std::string& path)
{
    NSString* file = [NSString stringWithUTF8String:path.c_str()];
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
            return ValueMap();
        }
    }
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:url options:nil];
    NSArray *metadata = [asset commonMetadata];
    ValueMap ccMetadata = ValueMap();
    ccMetadata["Duration"] = Value(getSoundDuration(path));
    for(AVMetadataItem* item in metadata) 
    {
        //Get the relevant metadata, currently title and artist
        if([[item commonKey] isEqual:@"title"])
        {
            ccMetadata["Title"] = Value([item stringValue].UTF8String);
        }
        else if([[item commonKey] isEqual:@"artist"])
        {
            ccMetadata["Author"] = Value([item stringValue].UTF8String);
        }
        //Uncomment to see what key/value are available in your files
        //log("key = %s, value = %s", [item commonKey].UTF8String, [item stringValue].UTF8String);
    }
    return ccMetadata;
}
