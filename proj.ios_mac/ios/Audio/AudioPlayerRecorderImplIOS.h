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

// original code from http://stackoverflow.com/questions/1010343/how-do-i-record-audio-on-iphone-with-avaudiorecorder

#import <AVFoundation/AVFoundation.h>

@interface AudioPlayerRecorderImpl : NSObject <AVAudioPlayerDelegate, AVAudioRecorderDelegate>
{
	AVAudioPlayer *audioPlayer;
    
    NSURL *url1;
    NSURL *url2;
	AVAudioRecorder *audioRecorder1;
	AVAudioRecorder *audioRecorder2; //there are 2 recorder to be able to prepare the next recorder while still being able to play last recorded sound
	BOOL nextRecorder; // YES = 1, NO = 2
	NSError *error;
	int recordEncoding;
	float desiredPlaybackRate;
    
	enum
	{
		ENC_AAC = 1,
		ENC_ALAC = 2,
		ENC_IMA4 = 3,
		ENC_ILBC = 4,
		ENC_ULAW = 5,
		ENC_PCM = 6,
	} encodingTypes;
}

+ (AudioPlayerRecorderImpl*) sharedAudio;

@property (nonatomic, readonly) BOOL isRecording;
@property (nonatomic, readonly) BOOL canPlay;
@property (nonatomic, readonly) BOOL isPlaying;
@property (nonatomic, readonly) float recordingDuration;
@property (nonatomic) float playbackRate;

+ (float) getSoundDuration:(NSString*)file;

- (void) setRecordEnabled:(BOOL)enabled;

- (void) startRecording;
- (void) stopRecording:(NSString*)file;
//- (BOOL) canPlay:(NSString*)file;
- (void) setPlayFile:(NSString*)file;
- (void) deleteFile:(NSString*)file;
- (float) play:(float)startTime volume:(float)volume;
- (float) playIndependentFile:(NSString*)file volume:(float)volume;
- (void) stopPlaying;
- (void) fadeVolumeOut;

- (void) play;
- (void) pause;
- (void) restart;

- (void) setNumberOfLoops:(int)loops;
//- (void) checkLowSound;

@end

