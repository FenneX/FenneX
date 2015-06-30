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

@interface AudioPlayerRecorderImpl (Private)

- (void) prepareNextRecording;

@end


@implementation AudioPlayerRecorderImpl (Private)

- (void) prepareNextRecording
{
	nextRecorder = !nextRecorder;
	AVAudioRecorder* audioRecorder = nextRecorder ? audioRecorder1 : audioRecorder2;
	if([audioRecorder prepareToRecord])
	{
#if VERBOSE_AUDIO
		NSLog(@"Preparing next recording");
#endif
	}
#if VERBOSE_AUDIO
	else
	{
		int errorCode = CFSwapInt32HostToBig ((uint32_t)[error code]);
		NSLog(@"Error with recording : %@ [%4.4s])" , [error localizedDescription], (char*)&errorCode);
	}
#endif
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
    //try sound in bundle first
    NSURL* url = [[NSBundle mainBundle] URLForResource:[file stringByDeletingPathExtension] withExtension:@"mp3"];
    if(url == nil)
    {
        //try to load recorded sound if it's not in bundle
        url = [NSURL fileURLWithPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file]];
        if(url == nil)
        {
#if VERBOSE_AUDIO
            NSLog(@"Warning : file %@ does not exist in getSoundDuration", file);
#endif
            return 0;
        }
    }
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
		NSLog(@"Recording...");
#endif
	}
#if VERBOSE_AUDIO
	else
	{
        int audioIsRecording = audioRecorder.isRecording ? 1 : 0;
		NSLog(@"Error when starting record, is recording ? %d", audioIsRecording);
	}
#endif
}

- (void) stopRecording:(NSString*)file
{
    error = nil;
#if VERBOSE_AUDIO
	NSLog(@"Stop recording");
#endif
	AVAudioRecorder* audioRecorder = nextRecorder ? audioRecorder1 : audioRecorder2;
	[audioRecorder stop];
#if VERBOSE_AUDIO
    if(error)
    {
        NSLog(@"Error when stopping record : %@", error.localizedDescription);
    }
    NSLog(@"Audio recorder url : %@", audioRecorder.url);
#endif
    NSURL* url = nextRecorder ? url1 : url2;
    NSURL* newUrl = [NSURL fileURLWithPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file]];
    if([[NSFileManager defaultManager] fileExistsAtPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file]])
    {
        [[NSFileManager defaultManager] removeItemAtPath:[[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file] error:nil];
    }
    BOOL result = [[NSFileManager defaultManager] moveItemAtURL:url toURL:newUrl error:&error];
#if VERBOSE_AUDIO
    if(!result)
    {
        NSLog(@"Error when moving item : %@, source exists : %@", error.localizedDescription, [[NSFileManager defaultManager] fileExistsAtPath:url.absoluteString] ? @"YES" : @"NO");
    }
#endif
    [self setPlayFile:file];
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
            return;
        }
    }
	audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
#if VERBOSE_AUDIO
    if(error)
    {
        NSLog(@"Error while trying to init audio : %@ for file : %@, full url : %@", error.localizedDescription, file, [url filePathURL]);
    }
    else
    {
        NSLog(@"playing file : %@, full url : %@", file, [url filePathURL]);
    }
#endif
    if(audioPlayer != nil)
    {
        audioPlayer.delegate = self;
        audioPlayer.numberOfLoops = 0;
    }
}

- (void) deleteFile:(NSString*)file
{
    NSString* path = [[[NSFileManager defaultManager] applicationSupportDirectory] stringByAppendingPathComponent:file];
    [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
}

- (float) play:(float)startTime
{
	if(audioPlayer != nil)
	{
        audioPlayer.currentTime = startTime;
        if(desiredPlaybackRate != 1)
        {
            audioPlayer.enableRate = YES;
            audioPlayer.rate = desiredPlaybackRate;
        }
		if([audioPlayer play])
        {
#if VERBOSE_AUDIO
            NSLog(@"playing for duration : %f, total duration : %f", [audioPlayer duration] - startTime, [audioPlayer duration]);
#endif
            return [audioPlayer duration] - startTime;
        }
#if VERBOSE_AUDIO
        else
        {
            NSLog(@"failed to play");
        }
#endif
	}
    return 0;
}

- (float) playIndependentFile:(NSString*)file
{
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
            return 0;
        }
    }
	AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
#if VERBOSE_AUDIO
    if(error)
    {
        NSLog(@"Error while trying to init audio : %@ for file : %@, full url : %@", error.localizedDescription, file, [url filePathURL]);
    }
    else
    {
        NSLog(@"playing file : %@, full url : %@", file, [url filePathURL]);
    }
#endif
    if(player != nil)
    {
        player.delegate = self;
        if(desiredPlaybackRate != 1)
        {
            player.enableRate = YES;
            player.rate = desiredPlaybackRate;
        }
		if([player play])
        {
#if VERBOSE_AUDIO
            NSLog(@"playing for duration : %f", [player duration]);
#endif
            return [player duration];
        }
#if VERBOSE_AUDIO
        else
        {
            NSLog(@"failed to play");
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
        NSLog(@"Stop playing");
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
    NSLog(@"Recorder number : %d ended successfully : %@", recorder == audioRecorder1 ? 1 : 2, flag ? @"YES" : @"NO");
#endif
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError *)localError
{
#if VERBOSE_AUDIO
    NSLog(@"Error with recorder : %@, failure reason : %@, recorder number : %d", [localError localizedDescription], [localError localizedFailureReason], recorder == audioRecorder1 ? 1 : 2);
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

