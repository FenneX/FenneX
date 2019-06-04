//
//  TTS.h
//  Madeleine
//
//  Created by François Dupayrat on 13/05/15.
//
//

#ifndef Madeleine_TTS_h
#define Madeleine_TTS_h

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>


@interface TTS : NSObject <AVSpeechSynthesizerDelegate>
{
    AVSpeechSynthesizer* engine;
    AVSpeechUtterance* lastUtterance;
    int lastID;
    float desiredPlayRate;
}


@property (nonatomic) float playRate;

- (void) speakText:(NSArray*)text callbackID:(int)callbackID;
- (void) stopSpeakText;
- (bool) isSpeaking;

- (void)speechSynthesizer:(AVSpeechSynthesizer *)synthesizer didFinishSpeechUtterance:(AVSpeechUtterance *)utterance;

+ (TTS*) sharedTTS;


@end

#endif
