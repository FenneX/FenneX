//
//  TTSWrapper.cpp
//  ClassIt iOS
//
//  Created by François Dupayrat on 24/04/2019.
//

#include "TTSWrapper.h"
#include <utility>

float estimateSpeechDuration(std::string text)
{
    if(text.empty()) return 0;
    size_t count = 0;
    bool isspace1, isspace2 = true;
    for (auto c : text) {
        isspace1 = isspace2;
        isspace2 = isspace(c);
        count += (isspace1 && !isspace2);
    }
    //Right now, we don't have any WPM values for speech
    return count / getCurrentVoiceWPM() * 60 * getTTSPlayRate();
}
