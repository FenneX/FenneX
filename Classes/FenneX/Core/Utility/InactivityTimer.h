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

#ifndef InactivityTimer_h
#define InactivityTimer_h

#include "cocos2d.h"
#include "Pausable.h"

/**
 * This class is used to handle inactivity time and launch inactivity event
 * Will launch an "UserInactivityDetected" event when a inactivity is longer than the timeDuration set by the "setTimerDuration" function
 * If the timer duration is set to -1 (default value), no event will be launched 
 * For now it only handle one InactivityTimer at a time
 * When you leave the app, the timer is reset, so when you come back it start again
 **/
class InactivityTimer : public FenneX::Pausable, public cocos2d::Ref
{
public:
    static InactivityTimer* getInstance();
    virtual void update(float delta);
    
    static void resetTimer();
    static void setTimerDuration(int timer);
    
private:
    InactivityTimer();
    
    float timePassedSinceLastActivity = 0;
    bool eventLaunched = false;
};

#endif /* InactivityTimer_h */
