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

#import "AudioPlayerRecorderImplIOS.h"
#import "NSFileManager+ApplicationSupport.h"

#include "cocos2d.h"
#include "AppMacros.h"
#include "AudioPlayerRecorder.h"
#include "FileUtility.h"

@interface AudioPlayerRecorderImpl (Private)

- (void) prepareNextRecording;
+ (NSURL*) getFullPath:(NSString*)file;

@end


@implementation AudioPlayerRecorderImpl (Private)

- (void) prepareNextRecording
{
	nextRecorder = !nextRecorder;
	AVAudioRecorder* audioRecorder = nextRecorder ? audioRecorder1 : audioRecorder2;
	if([audioRecorder prepareToRecord])
	{
#if VERBOSE_AUDIO
		CCLOG("Preparing next recording");
#endif
	}
#if VERBOSE_AUDIO
	else
	{
		int errorCode = CFSwapInt32HostToBig ((uint32_t)[error code]);
		CCLOG("Error with recording : %s [%4.4s])" , [[error localizedDescription] UTF8String], (char*)&errorCode);
	}
#endif
}

+ (NSURL*) getFullPath:(NSString*)file
{
    //try sound in bundle first (legacy behavior)
    NSURL* url = [[NSBundle mainBundle] URLForResource:[file stringByDeletingPathExtension] withExtension:@"mp3"];
    if(url == nil)
    {
        //try to load recorded sound if it's not in bundle
        std::string fullPath = findFullPath([file UTF8String]);
        if(fullPath.empty())
        {
#if VERBOSE_AUDIO
            CCLOG("Warning : audio file %s does not exist", [file UTF8String]);
#endif
            return nil;
        }
        url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%s", fullPath.c_str()]];
    }
    return url;
}

@end


@implementation AudioPlayerRecorderImpl

static AudioPlayerRecorderImpl* _sharedAudio = nil;

+ (AudioPlayerRecorderImpl*) sharedAudio
{
	@synchronized([AudioPlayerRecorderImpl class])
	{
		if (!_sharedAudio)
			[[self alloc] init];
		
		return _sharedAudio;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([AudioPlayerRecorderImpl class])
	{
		NSAssert(_sharedAudio == nil, @"Attempted to allocate a second instance of AudioPlayerRecorder singleton.");
		_sharedAudio = [super alloc];
		return _sharedAudio;
	}
	
	return nil;
}

+ (float) getSoundDuration:(NSString*)file
{
    NSError *error;
    NSURL* url = [AudioPlayerRecorderImpl getFullPath:file];
    if(url == nil) return 0;
    AVAudioPlayer* avAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
    
    double duration = avAudioPlayer.duration;
    [avAudioPlayer release];
    return duration;
}

- (BOOL) isRecording
{
	return [audioRecorder1 isRecording] || [audioRecorder2 isRecording];
}

- (BOOL) canPlay
{
    return audioPlayer != nil;
}

- (BOOL) isPlaying
{
	return [audioPlayer isPlaying];
}

- (float) recordingDuration
{
    return audioPlayer != nil ? audioPlayer.duration : 0;
}

- (float) playbackRate
{
    return desiredPlaybackRate;
}

- (void) setPlaybackRate:(float)rate
{
    //We can't modify the rate of a sound while playing it anyway
    desiredPlaybackRate = rate;
}

- (id) init
{
	self = [super init];
	if(self != nil)
	{
		recordEncoding = ENC_AAC;
        audioPlayer = nil;
        
        desiredPlaybackRate = 1;
		error = nil;
        
        [self setRecordEnabled:NO];
	}
	return self;
}

- (void) setRecordEnabled:(BOOL)enabled
{
    AVAudioSession * audioSession = [AVAudioSession sharedInstance];
    //Set the general audio session category
    [audioSession setCategory: enabled ? AVAudioSessionCategoryPlayAndRecord : AVAudioSessionCategorySoloAmbient error: &error];
    
    if(enabled)
    {
        //Make the default sound route for the session be to use the speaker
        UInt32 doChangeDefaultRoute = 1;
        AudioSessionSetProperty (kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, sizeof (doChangeDefaultRoute), &doChangeDefaultRoute);
    }
    
    //Activate the customized audio session
    [audioSession setActive: YES error: &error];
    if(enabled)
    {
		NSMutableDictionary* recordSettings = [[[NSMutableDictionary alloc] init] autorelease];
		[recordSettings setValue :[NSNumber numberWithInt:kAudioFormatMPEG4AAC] forKey:AVFormatIDKey]; //hopefully it will be easier to change the encoding later
		//[recordSettings setValue :[NSNumber numberWithInt:kAudioFormatAppleIMA4] forKey:AVFormatIDKey];
		[recordSettings setValue:[NSNumber numberWithFloat:44100.0] forKey:AVSampleRateKey];
		[recordSettings setValue:[NSNumber numberWithInt: 1] forKey:AVNumberOfChannelsKey]; //stereo isn't needed so 1 instead of 2
		
		url1 = [NSURL fileURLWithPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingString:@"/record1.caf"]];
		url2 = [NSURL fileURLWithPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingString:@"/record2.caf"]];
		
		audioRecorder1 = [[AVAudioRecorder alloc] initWithURL:url1 settings:recordSettings error:&error];
        audioRecorder1.delegate = self;
		audioRecorder2 = [[AVAudioRecorder alloc] initWithURL:url2 settings:recordSettings error:&error];
        audioRecorder2.delegate = self;
		[self prepareNextRecording];
    }
    else
    {
        [audioRecorder1 release];
        [audioRecorder2 release];
        url1 = NULL;
        url2 = NULL;
        audioRecorder1 = NULL;
        audioRecorder2 = NULL;
    }
}

- (void) startRecording
{
	AVAudioRecorder* audioRecorder = nextRecorder ? audioRecorder1 : audioRecorder2;
    error = nil;
    
    if([[NSFileManager defaultManager] fileExistsAtPath:audioRecorder.url.absoluteString])
    {
        [[NSFileManager defaultManager] removeItemAtPath:audioRecorder.url.absoluteString error:nil];
    }
	if([audioRecorder record])
	{
#if VERBOSE_AUDIO
		CCLOG("Recording...");
#endif
	}
#if VERBOSE_AUDIO
	else
	{
        int audioIsRecording = audioRecorder.isRecording ? 1 : 0;
		CCLOG("Error when starting record, is recording ? %d", audioIsRecording);
	}
#endif
}

- (void) stopRecording:(NSString*)fullPath
{
    error = nil;
#if VERBOSE_AUDIO
	CCLOG("Stop recording");
#endif
	AVAudioRecorder* audioRecorder = nextRecorder ? audioRecorder1 : audioRecorder2;
	[audioRecorder stop];
#if VERBOSE_AUDIO
    if(error)
    {
        CCLOG("Error when stopping record : %s", [error.localizedDescription UTF8String]);
    }
    CCLOG("Audio recorder url : %s", [audioRecorder.url.absoluteString  UTF8String]);
#endif
    if([[NSFileManager defaultManager] fileExistsAtPath:fullPath])
    {
        [[NSFileManager defaultManager] removeItemAtPath:fullPath error:nil];
    }
    NSURL* recordUrl = nextRecorder ? url1 : url2;
    BOOL result = [[NSFileManager defaultManager] moveItemAtURL:recordUrl toURL:[NSURL fileURLWithPath:fullPath] error:&error];
#if VERBOSE_AUDIO
    if(!result)
    {
        CCLOG("Error when moving item : %s, source exists : %s", [error.localizedDescription UTF8String], [[NSFileManager defaultManager] fileExistsAtPath:recordUrl.absoluteString] ? "YES" : "NO");
    }
#endif
    [self setPlayFile:fullPath];
	[self prepareNextRecording];
}

/*- (BOOL) canPlay:(NSString*)file
 {
 return [[NSFileManager defaultManager] fileExistsAtPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file]];
 }*/

- (void) setPlayFile:(NSString *)file
{
	if(audioPlayer)
	{
		[audioPlayer stop];
        [audioPlayer setDelegate:nil];
		[audioPlayer release];
        audioPlayer = nil;
	}
    NSURL* url = [AudioPlayerRecorderImpl getFullPath:file];
    if(url == nil) return;
	audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
#if VERBOSE_AUDIO
    if(error)
    {
        CCLOG("Error while trying to init audio : %s for file : %s, full url : %s", [error.localizedDescription UTF8String], [file UTF8String], [[url absoluteString] UTF8String]);
    }
    else
    {
        CCLOG("playing file : %s, full url : %s", [file UTF8String], [[url absoluteString] UTF8String]);
    }
#endif
    if(audioPlayer != nil)
    {
        audioPlayer.delegate = self;
        audioPlayer.numberOfLoops = 0;
    }
}

- (float) play:(float)startTime volume:(float)volume
{
	if(audioPlayer != nil)
	{
        audioPlayer.currentTime = startTime;
        audioPlayer.volume = volume;
        if(desiredPlaybackRate != 1)
        {
            audioPlayer.enableRate = YES;
            audioPlayer.rate = desiredPlaybackRate;
        }
		if([audioPlayer play])
        {
#if VERBOSE_AUDIO
            CCLOG("playing for duration : %f, total duration : %f", [audioPlayer duration] - startTime, [audioPlayer duration]);
#endif
            return [audioPlayer duration] - startTime;
        }
#if VERBOSE_AUDIO
        else
        {
            CCLOG("failed to play");
        }
#endif
	}
    return 0;
}

- (float) playIndependentFile:(NSString*)file volume:(float)volume
{
    NSURL* url = [AudioPlayerRecorderImpl getFullPath:file];
    if(url == nil) return 0;
	AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
#if VERBOSE_AUDIO
    if(error)
    {
        CCLOG("Error while trying to init audio : %s for file : %s, full url : %s", [error.localizedDescription UTF8String], [file UTF8String], [[url absoluteString] UTF8String]);
    }
    else
    {
        CCLOG("playing file : %s, full url : %s", [file UTF8String], [[url absoluteString] UTF8String]);
    }
#endif
    if(player != nil)
    {
        player.delegate = self;
        player.volume = volume;
        if(desiredPlaybackRate != 1)
        {
            player.enableRate = YES;
            player.rate = desiredPlaybackRate;
        }
		if([player play])
        {
#if VERBOSE_AUDIO
            CCLOG("playing for duration : %f", [player duration]);
#endif
            return [player duration];
        }
#if VERBOSE_AUDIO
        else
        {
            CCLOG("failed to play");
        }
#endif
    }
    return 0;
}

- (void) stopPlaying
{
	if(audioPlayer != nil)
    {
#if VERBOSE_AUDIO
        CCLOG("Stop playing");
#endif
        [audioPlayer stop];
        audioPlayer.volume = 1.0;
    }
}

- (void) fadeVolumeOut
{
    if (audioPlayer.volume > 0.1)
    {
        audioPlayer.volume = audioPlayer.volume - 0.1;
        [self performSelector:@selector(fadeVolumeOut) withObject:nil afterDelay:0.1];
    }
    else
    {
        [self stopPlaying];
    }
}

- (void) play
{
    if(desiredPlaybackRate != 1)
    {
        audioPlayer.enableRate = YES;
        audioPlayer.rate = desiredPlaybackRate;
    }
    [audioPlayer play];
}

- (void) pause
{
    [audioPlayer pause];
}

- (void) restart
{
    [audioPlayer setCurrentTime:0.0];
    if(![self isPlaying])
    {
        [self play];
    }
}

- (void) audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag
{
    if(flag && player == audioPlayer)
    {
        notifyPlayingSoundEnded();
    }
    else if(player != audioPlayer)
    {
        [audioPlayer setDelegate:nil];
        [player autorelease];
    }
}

- (void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag
{
#if VERBOSE_AUDIO
    CCLOG("Recorder number : %d ended successfully : %s", recorder == audioRecorder1 ? 1 : 2, flag ? "YES" : "NO");
#endif
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError *)localError
{
#if VERBOSE_AUDIO
    CCLOG("Error with recorder : %s, failure reason : %s, recorder number : %d", [[localError localizedDescription] UTF8String], [[localError localizedFailureReason] UTF8String], recorder == audioRecorder1 ? 1 : 2);
#endif
}

/*
 - (void) checkLowSound
 {
 float level = [MPMusicPlayerController applicationMusicPlayer].volume;
 if(level == 0)
 {
 [[LayoutHandler sharedLayoutHandler] createMessage:@"The volume is muted, please activate it"];
 }
 else if(level < 0.5)
 {
 [[LayoutHandler sharedLayoutHandler] createMessage:@"The volume is low, please increase it"];
 }
 else if([self isPlaying] && level < 1)
 {
 [[LayoutHandler sharedLayoutHandler] createMessage:@"If you don't hear well, please increase the volume"];
 }
 }*/

- (void)dealloc
{
	[audioPlayer release];
	[audioRecorder1 release];
	[audioRecorder2 release];
	[super dealloc];
}

@end

