//
//  TTS.m
//  Madeleine
//
//  Created by François Dupayrat on 13/05/15.
//
//

#import "TTS.h"
#include "TTSWrapper.h"

@implementation TTS

static TTS* _sharedTTS = nil;

+ (TTS*) sharedTTS
{
    @synchronized([TTS class])
    {
        if (!_sharedTTS)
            [[self alloc] init];
        
        return _sharedTTS;
    }
    
    return nil;
}

+ (id) alloc
{
    @synchronized([TTS class])
    {
        NSAssert(_sharedTTS == nil, @"Attempted to allocate a second instance of TTS singleton.");
        _sharedTTS = [super alloc];
        return _sharedTTS;
    }
    
    return nil;
}

- (id) init
{
    self = [super init];
    if (self != nil)
    {
        engine = [[AVSpeechSynthesizer alloc] init];
        [engine setDelegate:self];
    }
    return self;
}

- (void) speakText:(NSArray*)text callbackID:(int)callbackID
{
    lastID = callbackID;
    
    for (NSString* t in text)
    {
        AVSpeechUtterance* utterance = [AVSpeechUtterance speechUtteranceWithString:t];
        //Magical number ... Thanks Apple for changing the speed on iOS 8
        //Default is 0.5 (way too fast), minimum is 0, maximum is 1
        if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_7_1)
        {
            utterance.rate = 0.15;
        }
        else
        {
            utterance.rate = 0.3;
        }
        [engine speakUtterance:utterance];
        lastUtterance = utterance;
    }
}

- (void) stopSpeakText
{
    [engine stopSpeakingAtBoundary:AVSpeechBoundaryImmediate];
}

- (bool) isSpeaking
{
    return engine.isSpeaking;
}

- (void)speechSynthesizer:(AVSpeechSynthesizer *)synthesizer didFinishSpeechUtterance:(AVSpeechUtterance *)utterance
{
    if(synthesizer == engine && utterance == lastUtterance)
    {
        notifyTTSDone(lastID);
    }
}

@end